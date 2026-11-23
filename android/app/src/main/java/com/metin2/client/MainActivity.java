package com.metin2.client;

import android.app.Activity;
import android.graphics.Color;
import android.graphics.Canvas;
import android.graphics.Paint;
import android.opengl.GLSurfaceView;
import android.content.Context;
import android.text.InputType;
import android.os.Bundle;
import android.os.SystemClock;
import android.os.StatFs;
import android.view.Gravity;
import android.view.GestureDetector;
import android.view.KeyEvent;
import android.view.MotionEvent;
import android.view.ScaleGestureDetector;
import android.view.View;
import android.view.ViewConfiguration;
import android.view.Window;
import android.view.WindowManager;
import android.view.inputmethod.BaseInputConnection;
import android.view.inputmethod.EditorInfo;
import android.view.inputmethod.InputConnection;
import android.view.inputmethod.InputMethodManager;
import android.widget.Button;
import android.widget.LinearLayout;
import android.widget.ProgressBar;
import android.widget.TextView;
import android.widget.FrameLayout;

import java.io.BufferedInputStream;
import java.io.BufferedReader;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.net.HttpURLConnection;
import java.net.URL;
import java.nio.charset.StandardCharsets;
import java.security.MessageDigest;
import java.util.ArrayList;
import java.util.List;
import java.util.Locale;
import java.util.HashMap;
import java.util.Map;
import java.util.concurrent.CountDownLatch;
import java.util.concurrent.TimeUnit;

import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.egl.EGL10;
import javax.microedition.khronos.egl.EGLDisplay;
import javax.microedition.khronos.opengles.GL10;

public class MainActivity extends Activity {
    private static final long BACKGROUND_LOGOUT_MS = 60_000L;
    private static final String CONTENT_ROOT = BuildConfig.CONTENT_ROOT;
    private static final String[] ROOT_ASSETS = {
            "channel.inf", "game1.cfg", "loca.cfg", "locale.cfg",
            "locale_list.txt", "metin2.cfg", "mouse.cfg"
    };
    private static final String[] BUNDLED_PACK_OVERRIDES = {
            "root.data", "root.index"
    };
    private static final String[] REQUIRED_GAME_PACK_FILES = {
            "pack/root.data", "pack/root.index",
            "pack/locale.data", "pack/locale.index",
            "pack/gamedata.data", "pack/gamedata.index"
    };

    private GLSurfaceView glView;
    private ProgressBar totalProgress;
    private TextView statusText;
    private TextView detailText;
    private Button retryButton;
    private volatile boolean cancelled;
    private GestureDetector gestureDetector;
    private ScaleGestureDetector scaleGestureDetector;
    private TouchControlsView touchControls;
    private boolean cameraGesture;
    private float cameraLastX;
    private float cameraLastY;
    private long backgroundedAtMs;
    private volatile boolean keyboardVisible;
    private volatile boolean keyboardHidePosted;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        requestWindowFeature(Window.FEATURE_NO_TITLE);
        getWindow().setFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN,
                WindowManager.LayoutParams.FLAG_FULLSCREEN);
        getWindow().addFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);
        stageWritableConfiguration();
        showDownloader();
        startContentCheck();
    }

    private void showDownloader() {
        LinearLayout root = new LinearLayout(this);
        root.setOrientation(LinearLayout.VERTICAL);
        root.setGravity(Gravity.CENTER);
        root.setPadding(80, 50, 80, 50);
        root.setBackgroundColor(Color.rgb(5, 10, 22));

        TextView title = new TextView(this);
        title.setText("LUPUS2");
        title.setTextColor(Color.rgb(225, 181, 72));
        title.setTextSize(34);
        title.setGravity(Gravity.CENTER);
        root.addView(title, new LinearLayout.LayoutParams(-1, -2));

        statusText = new TextView(this);
        statusText.setText("Se verifică fișierele jocului...");
        statusText.setTextColor(Color.WHITE);
        statusText.setTextSize(18);
        statusText.setGravity(Gravity.CENTER);
        LinearLayout.LayoutParams textParams = new LinearLayout.LayoutParams(-1, -2);
        textParams.setMargins(0, 28, 0, 14);
        root.addView(statusText, textParams);

        totalProgress = new ProgressBar(this, null, android.R.attr.progressBarStyleHorizontal);
        totalProgress.setMax(10000);
        root.addView(totalProgress, new LinearLayout.LayoutParams(-1, 28));

        detailText = new TextView(this);
        detailText.setTextColor(Color.LTGRAY);
        detailText.setTextSize(14);
        detailText.setGravity(Gravity.CENTER);
        LinearLayout.LayoutParams detailParams = new LinearLayout.LayoutParams(-1, -2);
        detailParams.setMargins(0, 12, 0, 0);
        root.addView(detailText, detailParams);

        retryButton = new Button(this);
        retryButton.setText("Reîncearcă");
        retryButton.setVisibility(View.GONE);
        retryButton.setOnClickListener(v -> startContentCheck());
        LinearLayout.LayoutParams buttonParams = new LinearLayout.LayoutParams(300, -2);
        buttonParams.setMargins(0, 20, 0, 0);
        root.addView(retryButton, buttonParams);
        setContentView(root);
    }

    private void startContentCheck() {
        cancelled = false;
        retryButton.setVisibility(View.GONE);
        statusText.setText("Se verifică fișierele jocului...");
        detailText.setText("");
        totalProgress.setProgress(0);
        new Thread(() -> {
            try {
                installBundledPackOverrides();
                List<ContentFile> files;
                try {
                    files = readManifest();
                } catch (IOException manifestError) {
                    if (hasLaunchableLocalContent()) {
                        runOnUiThread(this::startGame);
                        return;
                    }
                    throw manifestError;
                }
                long required = 0;
                for (ContentFile entry : files) {
                    if (isBundledPackOverride(entry.path))
                        continue;
                    File target = safeTarget(entry.path);
                    if (!target.isFile() || target.length() != entry.size)
                        required += entry.size;
                }
                ensureFreeSpace(required);
                downloadMissing(files);
                installBundledPackOverrides();
                runOnUiThread(this::startGame);
            } catch (Exception error) {
                runOnUiThread(() -> showDownloadError(error.getMessage()));
            }
        }, "Lupus2Content").start();
    }

    private boolean hasLaunchableLocalContent() {
        File root = getFilesDir();
        for (String path : REQUIRED_GAME_PACK_FILES) {
            File file = new File(root, path);
            if ((!file.isFile() || file.length() == 0) && !hasBundledPackFile(path))
                return false;
        }
        return true;
    }

    private boolean hasBundledPackFile(String path) {
        try (android.content.res.AssetFileDescriptor file = getAssets().openFd(path)) {
            return file.getLength() > 0;
        } catch (IOException missingAsset) {
            return false;
        }
    }

    private List<ContentFile> readManifest() throws Exception {
        if (CONTENT_ROOT.isEmpty())
            throw new IOException("Adresa pentru descărcarea fișierelor de joc nu este configurată.");

        HttpURLConnection connection = openConnection(CONTENT_ROOT + "manifest.txt", 0);
        if (connection.getResponseCode() != HttpURLConnection.HTTP_OK)
            throw new IOException("Serverul de actualizări a răspuns cu " + connection.getResponseCode());
        List<ContentFile> files = new ArrayList<>();
        try (BufferedReader reader = new BufferedReader(new InputStreamReader(
                connection.getInputStream(), StandardCharsets.UTF_8))) {
            String line;
            while ((line = reader.readLine()) != null) {
                line = line.trim();
                if (line.isEmpty() || line.startsWith("#"))
                    continue;
                String[] fields = line.split("\\t", 3);
                if (fields.length != 3)
                    throw new IOException("Manifest invalid");
                files.add(new ContentFile(fields[2], Long.parseLong(fields[1]), fields[0]));
            }
        } finally {
            connection.disconnect();
        }
        if (files.isEmpty())
            throw new IOException("Lista de fișiere este goală");
        return files;
    }

    private void downloadMissing(List<ContentFile> files) throws Exception {
        long total = 0;
        for (ContentFile entry : files)
            total += entry.size;
        long completed = 0;
        int index = 0;
        for (ContentFile entry : files) {
            if (cancelled)
                throw new IOException("Descărcare anulată");
            index++;
            if (isBundledPackOverride(entry.path)) {
                completed += entry.size;
                continue;
            }
            File target = safeTarget(entry.path);
            final int shownIndex = index;
            final long shownCompleted = completed;
            final long shownTotal = total;
            runOnUiThread(() -> {
                statusText.setText("Actualizare Lupus2");
                detailText.setText(String.format(Locale.ROOT, "%d/%d  %s", shownIndex, files.size(), entry.path));
                totalProgress.setProgress(progress(shownCompleted, shownTotal));
            });
            if (target.isFile() && target.length() == entry.size && sha256(target).equals(entry.sha256)) {
                completed += entry.size;
                continue;
            }
            downloadOne(entry, target, completed, total);
            if (target.length() != entry.size || !sha256(target).equals(entry.sha256)) {
                target.delete();
                throw new IOException("Verificarea a eșuat pentru " + entry.path);
            }
            completed += entry.size;
        }
        totalProgress.setProgress(10000);
    }

    private static boolean isBundledPackOverride(String path) {
        return "pack/root.data".equals(path) || "pack/root.index".equals(path);
    }

    private void installBundledPackOverrides() throws IOException {
        File packDir = new File(getFilesDir(), "pack");
        if (!packDir.isDirectory() && !packDir.mkdirs())
            throw new IOException("Nu pot crea folderul pack");

        byte[] buffer = new byte[128 * 1024];
        for (String name : BUNDLED_PACK_OVERRIDES) {
            File target = new File(packDir, name);
            File temporary = new File(packDir, name + ".bundled");
            try (InputStream input = new BufferedInputStream(getAssets().open("pack_override/" + name));
                 FileOutputStream output = new FileOutputStream(temporary, false)) {
                int count;
                while ((count = input.read(buffer)) != -1)
                    output.write(buffer, 0, count);
                output.getFD().sync();
            }
            if (target.exists() && !target.delete())
                throw new IOException("Nu pot actualiza " + name);
            if (!temporary.renameTo(target))
                throw new IOException("Nu pot instala " + name);
        }
    }

    private void downloadOne(ContentFile entry, File target, long completed, long total) throws Exception {
        File parent = target.getParentFile();
        if (parent == null || (!parent.isDirectory() && !parent.mkdirs()))
            throw new IOException("Nu pot crea folderul pentru " + entry.path);
        File part = new File(target.getPath() + ".part");
        long offset = part.isFile() ? part.length() : 0;
        if (offset > entry.size) {
            part.delete();
            offset = 0;
        }
        HttpURLConnection connection = openConnection(CONTENT_ROOT + encodePath(entry.path), offset);
        int code = connection.getResponseCode();
        if (offset > 0 && code != HttpURLConnection.HTTP_PARTIAL) {
            part.delete();
            offset = 0;
            connection.disconnect();
            connection = openConnection(CONTENT_ROOT + encodePath(entry.path), 0);
            code = connection.getResponseCode();
        }
        if (code != HttpURLConnection.HTTP_OK && code != HttpURLConnection.HTTP_PARTIAL)
            throw new IOException("HTTP " + code + " pentru " + entry.path);
        try (InputStream input = new BufferedInputStream(connection.getInputStream(), 128 * 1024);
             FileOutputStream output = new FileOutputStream(part, offset > 0)) {
            byte[] buffer = new byte[128 * 1024];
            long current = offset;
            long lastUpdate = 0;
            int count;
            while ((count = input.read(buffer)) != -1) {
                if (cancelled)
                    throw new IOException("Descărcare anulată");
                output.write(buffer, 0, count);
                current += count;
                long now = System.currentTimeMillis();
                if (now - lastUpdate > 150) {
                    lastUpdate = now;
                    final long done = completed + current;
                    runOnUiThread(() -> totalProgress.setProgress(progress(done, total)));
                }
            }
            output.getFD().sync();
        } finally {
            connection.disconnect();
        }
        if (part.length() != entry.size)
            throw new IOException("Descărcare incompletă pentru " + entry.path);
        if (target.exists() && !target.delete())
            throw new IOException("Nu pot înlocui " + entry.path);
        if (!part.renameTo(target))
            throw new IOException("Nu pot finaliza " + entry.path);
    }

    private HttpURLConnection openConnection(String address, long offset) throws IOException {
        HttpURLConnection connection = (HttpURLConnection) new URL(address).openConnection();
        connection.setConnectTimeout(15000);
        connection.setReadTimeout(30000);
        connection.setRequestProperty("User-Agent", "Lupus2-Android/1.0");
        connection.setRequestProperty("Accept-Encoding", "identity");
        if (offset > 0)
            connection.setRequestProperty("Range", "bytes=" + offset + "-");
        return connection;
    }

    private void ensureFreeSpace(long bytes) throws IOException {
        long safetyMargin = 256L * 1024L * 1024L;
        long free = new StatFs(getFilesDir().getPath()).getAvailableBytes();
        if (free < bytes + safetyMargin)
            throw new IOException("Spațiu insuficient. Sunt necesari încă " + formatBytes(bytes + safetyMargin - free));
    }

    private File safeTarget(String relativePath) throws IOException {
        File root = getFilesDir().getCanonicalFile();
        File target = new File(root, relativePath.replace('/', File.separatorChar)).getCanonicalFile();
        if (!target.getPath().startsWith(root.getPath() + File.separator))
            throw new IOException("Cale invalidă în manifest");
        return target;
    }

    private static String encodePath(String path) {
        String[] parts = path.split("/");
        StringBuilder result = new StringBuilder();
        for (String part : parts) {
            if (result.length() > 0)
                result.append('/');
            try {
                result.append(java.net.URLEncoder.encode(part, "UTF-8").replace("+", "%20"));
            } catch (Exception ignored) {
                result.append(part);
            }
        }
        return result.toString();
    }

    private static String sha256(File file) throws Exception {
        MessageDigest digest = MessageDigest.getInstance("SHA-256");
        try (InputStream input = new BufferedInputStream(new FileInputStream(file), 128 * 1024)) {
            byte[] buffer = new byte[128 * 1024];
            int count;
            while ((count = input.read(buffer)) != -1)
                digest.update(buffer, 0, count);
        }
        StringBuilder value = new StringBuilder(64);
        for (byte item : digest.digest())
            value.append(String.format(Locale.ROOT, "%02x", item & 0xff));
        return value.toString();
    }

    private static int progress(long done, long total) {
        return total <= 0 ? 0 : (int) Math.min(10000, done * 10000L / total);
    }

    private static String formatBytes(long value) {
        return String.format(Locale.ROOT, "%.1f GB", value / 1073741824.0);
    }

    private void showDownloadError(String message) {
        statusText.setText("Actualizarea nu a putut continua");
        detailText.setText(message == null ? "Eroare necunoscută" : message);
        retryButton.setVisibility(View.VISIBLE);
    }

    private void startGame() {
        glView = new GameSurfaceView(this);
        glView.setEGLContextClientVersion(3);
        glView.setEGLConfigChooser(new MultisampleConfigChooser());
        glView.setPreserveEGLContextOnPause(true);
        glView.setRenderer(new Renderer());

        gestureDetector = new GestureDetector(this, new GestureDetector.SimpleOnGestureListener() {
            @Override
            public boolean onDown(MotionEvent event) {
                return true;
            }

            @Override
            public void onLongPress(MotionEvent event) {
                final float x = event.getX();
                final float y = event.getY();
                glView.queueEvent(() -> NativeLib.touchEvent(4, x, y));
            }

            @Override
            public boolean onDoubleTap(MotionEvent event) {
                final float x = event.getX();
                final float y = event.getY();
                glView.queueEvent(() -> NativeLib.touchEvent(5, x, y));
                return true;
            }
        });

        scaleGestureDetector = new ScaleGestureDetector(this, new ScaleGestureDetector.SimpleOnScaleGestureListener() {
            @Override
            public boolean onScale(ScaleGestureDetector detector) {
                final float factor = detector.getScaleFactor();
                if (factor < 0.98f || factor > 1.02f)
                    glView.queueEvent(() -> NativeLib.touchEvent(6, factor, 0.0f));
                return true;
            }
        });

        glView.setOnTouchListener((view, event) -> dispatchGameTouch(event));
        FrameLayout gameRoot = new FrameLayout(this);
        gameRoot.addView(glView, new FrameLayout.LayoutParams(
                FrameLayout.LayoutParams.MATCH_PARENT, FrameLayout.LayoutParams.MATCH_PARENT));
        touchControls = new TouchControlsView();
        gameRoot.addView(touchControls, new FrameLayout.LayoutParams(
                FrameLayout.LayoutParams.MATCH_PARENT, FrameLayout.LayoutParams.MATCH_PARENT));
        setContentView(gameRoot);
    }

    private boolean dispatchGameTouch(MotionEvent event) {
        gestureDetector.onTouchEvent(event);
        scaleGestureDetector.onTouchEvent(event);
        final int action = event.getActionMasked();
        if (action == MotionEvent.ACTION_DOWN)
            hideKeyboard();
        float sumX = 0.0f;
        float sumY = 0.0f;
        for (int i = 0; i < event.getPointerCount(); ++i) {
            sumX += event.getX(i);
            sumY += event.getY(i);
        }
        final float x = sumX / event.getPointerCount();
        final float y = sumY / event.getPointerCount();
        if (action == MotionEvent.ACTION_POINTER_DOWN) {
            glView.queueEvent(() -> NativeLib.touchEvent(MotionEvent.ACTION_CANCEL, x, y));
            cameraGesture = true;
            cameraLastX = x;
            cameraLastY = y;
            glView.queueEvent(() -> NativeLib.touchEvent(7, 0.0f, 0.0f));
        } else if (cameraGesture && action == MotionEvent.ACTION_MOVE) {
            final float dx = x - cameraLastX;
            final float dy = y - cameraLastY;
            cameraLastX = x;
            cameraLastY = y;
            glView.queueEvent(() -> NativeLib.touchEvent(8, dx, dy));
        } else if (cameraGesture && (action == MotionEvent.ACTION_POINTER_UP ||
                action == MotionEvent.ACTION_UP || action == MotionEvent.ACTION_CANCEL)) {
            cameraGesture = false;
            glView.queueEvent(() -> NativeLib.touchEvent(9, 0.0f, 0.0f));
        } else if (event.getPointerCount() == 1 &&
                (action == MotionEvent.ACTION_DOWN || action == MotionEvent.ACTION_UP ||
                 action == MotionEvent.ACTION_MOVE || action == MotionEvent.ACTION_CANCEL)) {
            glView.queueEvent(() -> NativeLib.touchEvent(action, x, y));
        }
        if (action == MotionEvent.ACTION_UP) {
			final boolean loginInputWasFocused = NativeLib.isLoginTextInputFocused();
			glView.queueEvent(() -> {
				final boolean explicitlyRequested = NativeLib.consumeKeyboardRequest();
				final boolean mapReady = NativeLib.isMapReady();
				glView.post(() -> {
					if (explicitlyRequested || (!mapReady && !loginInputWasFocused))
						syncKeyboardAfterTouch(0);
					else
						hideKeyboard();
				});
			});
        }
        return true;
    }

    private void syncKeyboardAfterTouch(final int attempt) {
        if (glView == null)
            return;
        glView.postDelayed(() -> {
            // LoginWindow is created by the Python event queue. Depending on the
            // frame in which channel selection completes, its default ID focus can
            // appear a few frames after ACTION_UP. Poll briefly instead of losing
            // the one keyboard request for the entire login screen.
            if (NativeLib.isTextInputActive()) {
                if (glView instanceof GameSurfaceView)
                    ((GameSurfaceView)glView).showKeyboard();
                return;
            }
            if (attempt < 6)
                syncKeyboardAfterTouch(attempt + 1);
            else
                hideKeyboard();
        }, attempt == 0 ? 80 : 100);
    }

    private void hideKeyboard() {
        if (glView == null)
            return;
        InputMethodManager manager = (InputMethodManager)getSystemService(Context.INPUT_METHOD_SERVICE);
        if (manager != null)
            manager.hideSoftInputFromWindow(glView.getWindowToken(), 0);
        keyboardVisible = false;
        glView.clearFocus();
    }

    private class GameSurfaceView extends GLSurfaceView {
        GameSurfaceView(Context context) {
            super(context);
            setFocusableInTouchMode(true);
        }

        void showKeyboard() {
            requestFocus();
            InputMethodManager manager = (InputMethodManager)getSystemService(Context.INPUT_METHOD_SERVICE);
            if (manager != null) {
                // Focus may have moved from account to password. Recreate the
                // editor connection so Android immediately applies password mode.
                manager.restartInput(this);
                manager.showSoftInput(this, InputMethodManager.SHOW_IMPLICIT);
                keyboardVisible = true;
            }
        }

		@Override
		public boolean onKeyPreIme(int keyCode, KeyEvent event) {
			if (keyCode == KeyEvent.KEYCODE_BACK &&
					event.getAction() == KeyEvent.ACTION_UP && keyboardVisible) {
				keyboardVisible = false;
				queueEvent(() -> NativeLib.touchEvent(201, 0.0f, 0.0f));
			}
			return super.onKeyPreIme(keyCode, event);
		}

        @Override
        public boolean onCheckIsTextEditor() {
            return true;
        }

        @Override
        public InputConnection onCreateInputConnection(EditorInfo outAttrs) {
            final boolean secretInput = NativeLib.isTextInputSecret();
            final boolean loginInput = NativeLib.isLoginTextInputFocused();
            if (secretInput) {
                outAttrs.inputType = InputType.TYPE_CLASS_TEXT |
                        InputType.TYPE_TEXT_VARIATION_PASSWORD |
                        InputType.TYPE_TEXT_FLAG_NO_SUGGESTIONS;
            } else {
                outAttrs.inputType = InputType.TYPE_CLASS_TEXT |
                        InputType.TYPE_TEXT_VARIATION_VISIBLE_PASSWORD |
                        InputType.TYPE_TEXT_FLAG_NO_SUGGESTIONS;
            }
            outAttrs.imeOptions = (secretInput ? EditorInfo.IME_ACTION_DONE :
                    (loginInput ? EditorInfo.IME_ACTION_NEXT : EditorInfo.IME_ACTION_SEND)) |
                    EditorInfo.IME_FLAG_NO_EXTRACT_UI;
            return new BaseInputConnection(this, false) {
                private String composingText = "";

                private void replaceNativeComposingText(CharSequence text, boolean finished) {
                    final String value = text == null ? "" : text.toString();
                    final int removeCount = composingText.length();
                    composingText = finished ? "" : value;
                    glView.queueEvent(() -> {
                        for (int i = 0; i < removeCount; ++i)
                            NativeLib.textKey(8);
                        if (!value.isEmpty())
                            NativeLib.textInput(value);
                    });
                }

                @Override
                public boolean setComposingText(CharSequence text, int newCursorPosition) {
                    replaceNativeComposingText(text, false);
                    return true;
                }

                @Override
                public boolean commitText(CharSequence text, int newCursorPosition) {
                    replaceNativeComposingText(text, true);
                    return true;
                }

                @Override
                public boolean finishComposingText() {
                    composingText = "";
                    return true;
                }

                @Override
                public boolean deleteSurroundingText(int beforeLength, int afterLength) {
                    final int count = Math.max(1, beforeLength);
                    if (!composingText.isEmpty())
                        composingText = composingText.substring(0,
                                Math.max(0, composingText.length() - count));
                    glView.queueEvent(() -> {
                        for (int i = 0; i < count; ++i)
                            NativeLib.textKey(8);
                    });
                    return true;
                }

                @Override
                public boolean sendKeyEvent(KeyEvent event) {
                    if (event.getAction() != KeyEvent.ACTION_DOWN)
                        return true;

                    if (event.getKeyCode() == KeyEvent.KEYCODE_DEL) {
                        composingText = "";
                        glView.queueEvent(() -> NativeLib.textKey(8));
                        return true;
                    }

                    if (event.getKeyCode() == KeyEvent.KEYCODE_ENTER) {
                        composingText = "";
                        glView.queueEvent(() -> NativeLib.textKey(13));
                        glView.postDelayed(() -> {
                            if (!secretInput && NativeLib.isTextInputActive())
                                showKeyboard();
                            else
                                hideKeyboard();
                        }, 80);
                        return true;
                    }

                    final int unicode = event.getUnicodeChar();
                    if (unicode != 0) {
                        final String value = new String(Character.toChars(unicode));
                        composingText = "";
                        glView.queueEvent(() -> NativeLib.textInput(value));
                        return true;
                    }

                    return super.sendKeyEvent(event);
                }

                @Override
                public boolean performEditorAction(int actionCode) {
                    glView.queueEvent(() -> NativeLib.textKey(13));
                    glView.postDelayed(() -> {
                        if (!secretInput && NativeLib.isTextInputActive())
                            showKeyboard();
                        else
                            hideKeyboard();
                    }, 80);
                    return true;
                }
            };
        }
    }

    private void sendControl(int action) {
        if (glView != null)
            glView.queueEvent(() -> NativeLib.touchEvent(action, 0.0f, 0.0f));
    }

    private class TouchControlsView extends View {
        private final Paint paint = new Paint(Paint.ANTI_ALIAS_FLAG);
        private static final int PAGE_TOGGLE = -2;
        private int activePointer = -1;
        private int activeDirectionMask;
        private float joystickKnobX;
        private float joystickKnobY;
        private int quickPage;
        private int worldCameraPointer = -1;
        private float worldCameraDownX;
        private float worldCameraDownY;
        private boolean worldCameraMoved;
        private long lastWorldTapAt;
        private float lastWorldTapX;
        private float lastWorldTapY;
        private final float worldTapSlopSquared;
        private final float worldDoubleTapSlopSquared;
        private boolean nativeUiGesture;
        private final Map<Integer, Integer> heldButtons = new HashMap<>();

        TouchControlsView() {
            super(MainActivity.this);
            setWillNotDraw(false);
            final ViewConfiguration configuration = ViewConfiguration.get(MainActivity.this);
            // A phone finger jitters much more than a desktop cursor.  Keep
            // small movement classified as a tap and start rotating only after
            // a deliberate drag; otherwise the camera moves the actor away
            // from the picking ray before ACTION_UP.
            final float tapSlop = Math.max(42.0f,
                    configuration.getScaledTouchSlop() * 2.5f);
            final float doubleTapSlop = configuration.getScaledDoubleTapSlop();
            worldTapSlopSquared = tapSlop * tapSlop;
            worldDoubleTapSlopSquared = doubleTapSlop * doubleTapSlop;
        }

        private float joystickX() {
            return getWidth() * 0.13f;
        }

        private float joystickY() {
            // Keep the fixed stick above the PC "Creste status" control.
            return getHeight() * 0.70f;
        }

        private float joystickRadius() {
            return Math.max(76.0f, getHeight() * 0.090f);
        }

        private boolean isJoystickHit(float x, float y) {
            final float dx = x - joystickX();
            final float dy = y - joystickY();
            final float hit = joystickRadius() * 1.06f;
            return dx * dx + dy * dy <= hit * hit;
        }

        private float[][] quickButtonCenters() {
            final float attackX = getWidth() * 0.880f;
            final float attackY = getHeight() * 0.720f;
            final float layoutScale = getHeight() / 900.0f;
            final float[][] offsets = {
                    {-235.0f,   96.0f}, // 1
                    {-210.0f,  -31.0f}, // 2
                    {-137.0f, -137.0f}, // 3
                    { -31.0f, -210.0f}, // 4
                    {  96.0f, -235.0f}  // 5
            };
            final float[][] centers = new float[offsets.length][2];
            for (int i = 0; i < offsets.length; ++i) {
                centers[i][0] = attackX + layoutScale * offsets[i][0];
                centers[i][1] = attackY + layoutScale * offsets[i][1];
            }
            return centers;
        }

        private int buttonAt(float x, float y) {
            final float attackX = getWidth() * 0.880f;
            final float attackY = getHeight() * 0.720f;
            float dx = x - attackX;
            float dy = y - attackY;
            final float attackHit = Math.max(96.0f, getHeight() * 0.105f);
            if (dx * dx + dy * dy <= attackHit * attackHit)
                return 4; // SPACE

            final float[][] quick = quickButtonCenters();
            final float quickHit = Math.max(45.0f, getHeight() * 0.047f);
            for (int i = 0; i < quick.length; ++i) {
                dx = x - quick[i][0];
                dy = y - quick[i][1];
                if (dx * dx + dy * dy <= quickHit * quickHit)
                    return (quickPage == 0 ? 5 : 10) + i;
            }

            final float utilityHit = Math.max(43.0f, getHeight() * 0.045f);
            final float utilityX = attackX - 331.0f * (getHeight() / 900.0f);
            dx = x - utilityX;
            dy = y - quick[1][1];
            if (dx * dx + dy * dy <= utilityHit * utilityHit)
                return 15; // original PC Ctrl+G
            dx = x - utilityX;
            dy = y - quick[0][1];
            if (dx * dx + dy * dy <= utilityHit * utilityHit)
                return 16; // original PC Z

            // The visual lives in the original bottom taskbar, in the free
            // space between the HP/MP/EXP cluster and the quick slots.
            final float chatX = getWidth() * 0.335f;
            final float chatY = getHeight() * 0.970f;
            dx = x - chatX;
            dy = y - chatY;
            final float chatHit = Math.max(58.0f, getHeight() * 0.060f);
            if (dx * dx + dy * dy <= chatHit * chatHit)
                return 17; // original PC chat/IME return path

            final float pageHit = Math.max(38.0f, getHeight() * 0.040f);
            dx = x - getWidth() * 0.850f;
            dy = y - getHeight() * 0.855f;
            if (dx * dx + dy * dy <= pageHit * pageHit)
                return PAGE_TOGGLE;
            return -1;
        }

        private int directionMaskFor(float x, float y) {
            final float dx = x - joystickX();
            final float dy = y - joystickY();
            final float deadZone = joystickRadius() * 0.22f;
            int mask = 0;
            if (dy < -deadZone)
                mask |= 1 << 0; // UP
            else if (dy > deadZone)
                mask |= 1 << 1; // DOWN
            if (dx < -deadZone)
                mask |= 1 << 2; // LEFT
            else if (dx > deadZone)
                mask |= 1 << 3; // RIGHT
            return mask;
        }

        private void setDirectionMask(int directionMask) {
            if (activeDirectionMask == directionMask)
                return;
            for (int direction = 0; direction < 4; ++direction) {
                final int bit = 1 << direction;
                if ((activeDirectionMask & bit) != (directionMask & bit))
                    sendControl(((directionMask & bit) != 0 ? 100 : 101) + direction * 2);
            }
            activeDirectionMask = directionMask;
            invalidate();
        }

        private void updateJoystick(float x, float y) {
            final float centerX = joystickX();
            final float centerY = joystickY();
            final float maxTravel = joystickRadius() * 0.58f;
            float dx = x - centerX;
            float dy = y - centerY;
            final float distance = (float)Math.sqrt(dx * dx + dy * dy);
            if (distance > maxTravel && distance > 0.0f) {
                dx *= maxTravel / distance;
                dy *= maxTravel / distance;
            }
            joystickKnobX = centerX + dx;
            joystickKnobY = centerY + dy;
            setDirectionMask(directionMaskFor(x, y));
        }

        void releaseAllControls() {
            setDirectionMask(0);
            activePointer = -1;
            joystickKnobX = joystickX();
            joystickKnobY = joystickY();
            for (Integer held : heldButtons.values())
                sendControl(101 + held * 2);
            heldButtons.clear();
            if (worldCameraPointer != -1)
                sendControl(9);
            worldCameraPointer = -1;
            invalidate();
        }

        private boolean isTouchOverNativeUi(final float x, final float y) {
            if (glView == null)
                return false;

            final boolean[] result = { true };
            final CountDownLatch completed = new CountDownLatch(1);
            glView.queueEvent(() -> {
                result[0] = NativeLib.isTouchOverUi(x, y);
                completed.countDown();
            });
            try {
                // Window picking belongs to the GL/game thread.  A short wait on
                // ACTION_DOWN prevents the Android controls from stealing a
                // gesture that began on an inventory or skill slot.
                if (!completed.await(80, TimeUnit.MILLISECONDS))
                    return true;
            } catch (InterruptedException exception) {
                Thread.currentThread().interrupt();
                return true;
            }
            return result[0];
        }

        @Override
        public boolean onTouchEvent(MotionEvent event) {
			if (!NativeLib.isMapReady())
				return dispatchGameTouch(event);
            final int index = event.getActionIndex();
            final int id = event.getPointerId(index);
            final float x = event.getX(index);
            final float y = event.getY(index);
            final int action = event.getActionMasked();

            if (action == MotionEvent.ACTION_DOWN) {
                // PC windows always win over the mobile HUD.  The JNI picker
                // excludes only the full-screen 3D GameWindow itself.  The chat
                // shortcut is intentionally drawn directly on the PC taskbar,
                // so its hit area must be claimed before that taskbar consumes
                // the gesture as an otherwise inert desktop click.
                final boolean mobileChatShortcut = buttonAt(x, y) == 17;
                nativeUiGesture = !mobileChatShortcut && isTouchOverNativeUi(x, y);
				// World/mobile-control taps do not pass through AndroidTouchDown, so
				// they cannot release a focused PC EditLine there. Dismiss both sides
				// explicitly: Android's IME and the client's native text capture. This
				// removes the blinking caret while leaving the chat window itself open.
				if (!nativeUiGesture && !mobileChatShortcut) {
					if (NativeLib.isTextInputActive()) {
						// First outside tap while typing: stop typing, keep chat open.
						sendControl(201);
						hideKeyboard();
					} else {
						// A later outside tap with chat already unfocused closes the
						// original PC chat. If chat is closed, this is a no-op.
						sendControl(202);
					}
				}
                if (nativeUiGesture) {
                    releaseAllControls();
                    return dispatchGameTouch(event);
                }
            } else if (nativeUiGesture) {
                boolean handled = dispatchGameTouch(event);
                if (action == MotionEvent.ACTION_UP || action == MotionEvent.ACTION_CANCEL)
                    nativeUiGesture = false;
                return handled;
            }

            if ((action == MotionEvent.ACTION_DOWN || action == MotionEvent.ACTION_POINTER_DOWN) &&
                    isJoystickHit(x, y)) {
                if (activePointer == -1) {
                    activePointer = id;
                    updateJoystick(x, y);
                }
                invalidate();
                return true;
            }

            if (action == MotionEvent.ACTION_DOWN || action == MotionEvent.ACTION_POINTER_DOWN) {
                int button = buttonAt(x, y);
                if (button == PAGE_TOGGLE) {
                    quickPage = 1 - quickPage;
                    sendControl(200);
                    invalidate();
                    return true;
                } else if (button >= 0) {
                    heldButtons.put(id, button);
                    sendControl(100 + button * 2);
                    invalidate();
                    return true;
                }
            }

            if (action == MotionEvent.ACTION_POINTER_DOWN && worldCameraPointer != -1)
                worldCameraMoved = true;

            if (action == MotionEvent.ACTION_MOVE && activePointer != -1) {
                int p = event.findPointerIndex(activePointer);
                if (p >= 0)
                    updateJoystick(event.getX(p), event.getY(p));
                else {
                    setDirectionMask(0);
                    activePointer = -1;
                    joystickKnobX = joystickX();
                    joystickKnobY = joystickY();
                }
            }

            if (action == MotionEvent.ACTION_UP || action == MotionEvent.ACTION_CANCEL ||
                    action == MotionEvent.ACTION_POINTER_UP) {
                boolean ownedByControls = id == activePointer || heldButtons.containsKey(id);
                if (action == MotionEvent.ACTION_CANCEL)
                    ownedByControls = activePointer != -1 || !heldButtons.isEmpty();
                if (id == activePointer || action == MotionEvent.ACTION_CANCEL) {
                    setDirectionMask(0);
                    activePointer = -1;
                    joystickKnobX = joystickX();
                    joystickKnobY = joystickY();
                }
				Integer button = heldButtons.remove(id);
				if (button != null) {
					sendControl(101 + button * 2);
					if (button == 17)
						hideKeyboard();
				}
				if (action == MotionEvent.ACTION_CANCEL) {
					releaseAllControls();
                }
                invalidate();
                if (ownedByControls)
                    return true;
            }

            // The world no longer receives a desktop left click.  One finger
            // rotates the original PC camera, while movement is exclusively
            // driven by the fixed joystick.  Pinch keeps the PC mouse-wheel zoom.
            // A joystick/button pointer must not be interpreted as one half of
            // a pinch.  Camera dragging is still allowed concurrently.
            if (activePointer == -1 && heldButtons.isEmpty())
                scaleGestureDetector.onTouchEvent(event);
            if ((action == MotionEvent.ACTION_DOWN ||
                    action == MotionEvent.ACTION_POINTER_DOWN) &&
                    worldCameraPointer == -1) {
                if (action == MotionEvent.ACTION_DOWN)
                    hideKeyboard();
                worldCameraPointer = id;
                worldCameraDownX = x;
                worldCameraDownY = y;
                worldCameraMoved = false;
                cameraLastX = x;
                cameraLastY = y;
                sendControl(7);
                return true;
            }
            if (action == MotionEvent.ACTION_MOVE && worldCameraPointer != -1 &&
                    !scaleGestureDetector.isInProgress()) {
                final int pointerIndex = event.findPointerIndex(worldCameraPointer);
                if (pointerIndex >= 0) {
                    final float nextX = event.getX(pointerIndex);
                    final float nextY = event.getY(pointerIndex);
                    final float totalDx = nextX - worldCameraDownX;
                    final float totalDy = nextY - worldCameraDownY;
                    if (totalDx * totalDx + totalDy * totalDy > worldTapSlopSquared)
                        worldCameraMoved = true;
                    if (worldCameraMoved) {
                        final float dx = nextX - cameraLastX;
                        final float dy = nextY - cameraLastY;
                        cameraLastX = nextX;
                        cameraLastY = nextY;
                        if (dx != 0.0f || dy != 0.0f) {
                            final float moveX = dx;
                            final float moveY = dy;
                            glView.queueEvent(() -> NativeLib.touchEvent(8, moveX, moveY));
                        }
                    }
                }
                return true;
            }
            if ((action == MotionEvent.ACTION_UP || action == MotionEvent.ACTION_CANCEL ||
                    action == MotionEvent.ACTION_POINTER_UP) &&
                    (id == worldCameraPointer || action == MotionEvent.ACTION_CANCEL)) {
                worldCameraPointer = -1;
                sendControl(9);
                if (action == MotionEvent.ACTION_UP && !worldCameraMoved) {
                    final long now = SystemClock.uptimeMillis();
                    final float tapDx = x - lastWorldTapX;
                    final float tapDy = y - lastWorldTapY;
                    final boolean doubleTap = lastWorldTapAt != 0L &&
                            now - lastWorldTapAt <= ViewConfiguration.getDoubleTapTimeout() &&
                            tapDx * tapDx + tapDy * tapDy <= worldDoubleTapSlopSquared;
                    final int actorAction = doubleTap ? 11 : 10;
                    final float tapX = x;
                    final float tapY = y;
                    glView.queueEvent(() -> NativeLib.touchEvent(actorAction, tapX, tapY));
                    if (doubleTap) {
                        lastWorldTapAt = 0L;
                    } else {
                        lastWorldTapAt = now;
                        lastWorldTapX = x;
                        lastWorldTapY = y;
                    }
                }
                return true;
            }
            return true;
        }

        private boolean isHeld(int keyIndex) {
            return heldButtons.containsValue(keyIndex);
        }

        private void drawThemedButton(Canvas canvas, float x, float y, float radius,
                                      String label, boolean pressed, float textScale) {
            paint.setStyle(Paint.Style.FILL);
            paint.setColor(pressed ? 0xD04B4332 : 0xB5212223);
            canvas.drawCircle(x, y, radius, paint);
            paint.setColor(pressed ? 0x805F512F : 0x403B3529);
            canvas.drawCircle(x, y, radius * 0.82f, paint);
            paint.setStyle(Paint.Style.STROKE);
            paint.setStrokeWidth(Math.max(3.0f, getHeight() * 0.0042f));
            paint.setColor(pressed ? 0xFFF1D48A : 0xD6B59A5A);
            canvas.drawCircle(x, y, radius, paint);
            paint.setStyle(Paint.Style.FILL);
            paint.setColor(0xFFF2E8CE);
            paint.setTextAlign(Paint.Align.CENTER);
            paint.setTypeface(android.graphics.Typeface.create("serif", android.graphics.Typeface.BOLD));
            paint.setTextSize(Math.max(22.0f, getHeight() * textScale));
            canvas.drawText(label, x, y - (paint.ascent() + paint.descent()) * 0.5f, paint);
        }

        @Override
        protected void onDraw(Canvas canvas) {
            super.onDraw(canvas);
			// The Android view is input-only.  The visible HUD is rendered by the
			// client's GAME layer so native UI windows can cover it correctly.
			if (NativeLib.isMapReady())
				return;
			postInvalidateDelayed(400);
			if (!NativeLib.isMapReady())
				return;

            final float stickX = joystickX();
            final float stickY = joystickY();
            final float stickRadius = joystickRadius();
            if (activePointer == -1) {
                joystickKnobX = stickX;
                joystickKnobY = stickY;
            }
            paint.setStyle(Paint.Style.FILL);
            paint.setColor(0x9C202122);
            canvas.drawCircle(stickX, stickY, stickRadius, paint);
            paint.setStyle(Paint.Style.STROKE);
            paint.setStrokeWidth(Math.max(4.0f, getHeight() * 0.005f));
            paint.setColor(0xD6B59A5A);
            canvas.drawCircle(stickX, stickY, stickRadius, paint);
            paint.setStrokeWidth(Math.max(2.0f, getHeight() * 0.0025f));
            paint.setColor(0x607D6D48);
            canvas.drawCircle(stickX, stickY, stickRadius * 0.64f, paint);
            paint.setStyle(Paint.Style.FILL);
            paint.setColor(activePointer == -1 ? 0xC84A4945 : 0xE06A6047);
            canvas.drawCircle(joystickKnobX, joystickKnobY, stickRadius * 0.36f, paint);
            paint.setStyle(Paint.Style.STROKE);
            paint.setStrokeWidth(Math.max(3.0f, getHeight() * 0.0035f));
            paint.setColor(0xFFE3C878);
            canvas.drawCircle(joystickKnobX, joystickKnobY, stickRadius * 0.36f, paint);

            final float attackX = getWidth() * 0.90f;
            final float attackY = getHeight() * 0.755f;
            drawThemedButton(canvas, attackX, attackY,
                    Math.max(82.0f, getHeight() * 0.094f), "ATAC", isHeld(4), 0.030f);

			final float[][] quick = quickButtonCenters();
			final String[] labels = quickPage == 0
					? new String[] {"1", "2", "3", "4", "5"}
					: new String[] {"TAB", "F1", "F2", "F3", "F4"};
			for (int i = 0; i < quick.length; ++i) {
				final float sx = getWidth() * quick[i][0];
				final float sy = getHeight() * quick[i][1];
                final int keyIndex = (quickPage == 0 ? 5 : 10) + i;
                drawThemedButton(canvas, sx, sy,
                        Math.max(54.0f, getHeight() * 0.058f), labels[i],
                        isHeld(keyIndex), quickPage == 0 ? 0.027f : 0.020f);
			}

            drawThemedButton(canvas, getWidth() * 0.735f, getHeight() * 0.585f,
                    Math.max(51.0f, getHeight() * 0.054f), "CAL", isHeld(15), 0.020f);
            drawThemedButton(canvas, getWidth() * 0.735f, getHeight() * 0.715f,
                    Math.max(51.0f, getHeight() * 0.054f), "IA", isHeld(16), 0.023f);
            drawThemedButton(canvas, getWidth() * 0.805f, getHeight() * 0.835f,
                    Math.max(43.0f, getHeight() * 0.047f), quickPage == 0 ? ">" : "<",
                    false, 0.026f);
        }
    }

    private void stageWritableConfiguration() {
        for (String name : ROOT_ASSETS) {
            File destination = new File(getFilesDir(), name);
            if (destination.exists())
                continue;
            try (InputStream input = getAssets().open(name);
                 FileOutputStream output = new FileOutputStream(destination)) {
                byte[] buffer = new byte[8192];
                int count;
                while ((count = input.read(buffer)) != -1)
                    output.write(buffer, 0, count);
            } catch (IOException exception) {
                throw new IllegalStateException("Cannot stage " + name, exception);
            }
        }
		stageAssetTree("lib", new File(getFilesDir(), "lib"));
    }

	private void stageAssetTree(String assetPath, File destination) {
		try {
			String[] children = getAssets().list(assetPath);
			if (children != null && children.length != 0) {
				if (!destination.isDirectory() && !destination.mkdirs())
					throw new IOException("Cannot create " + destination);
				for (String child : children)
					stageAssetTree(assetPath + "/" + child, new File(destination, child));
				return;
			}
			if (destination.isFile())
				return;
			try (InputStream input = getAssets().open(assetPath);
				 FileOutputStream output = new FileOutputStream(destination)) {
				byte[] buffer = new byte[8192];
				int count;
				while ((count = input.read(buffer)) != -1)
					output.write(buffer, 0, count);
			}
		} catch (IOException exception) {
			throw new IllegalStateException("Cannot stage " + assetPath, exception);
		}
	}

    @Override
    protected void onPause() {
        backgroundedAtMs = SystemClock.elapsedRealtime();
        if (touchControls != null)
            touchControls.releaseAllControls();
        cameraGesture = false;
        if (glView != null)
            glView.onPause();
        super.onPause();
    }

    @Override
    public void onWindowFocusChanged(boolean hasFocus) {
        super.onWindowFocusChanged(hasFocus);
        if (!hasFocus && touchControls != null)
            touchControls.releaseAllControls();
        if (!hasFocus)
            cameraGesture = false;
    }

    @Override
    protected void onResume() {
        super.onResume();
        if (glView != null) {
            glView.onResume();
            if (backgroundedAtMs != 0L) {
                final long elapsed = Math.max(0L,
                        SystemClock.elapsedRealtime() - backgroundedAtMs);
                backgroundedAtMs = 0L;
                if (elapsed >= BACKGROUND_LOGOUT_MS)
                    glView.queueEvent(() -> NativeLib.resumeFromBackground(elapsed));
            }
        }
        if (touchControls != null)
            touchControls.invalidate();
    }

    @Override
    protected void onDestroy() {
        cancelled = true;
        super.onDestroy();
    }

    private static final class MultisampleConfigChooser implements GLSurfaceView.EGLConfigChooser {
        private static final int EGL_OPENGL_ES3_BIT_KHR = 0x40;

        @Override
        public EGLConfig chooseConfig(EGL10 egl, EGLDisplay display) {
            // Prefer the PC client's smooth polygon edges.  The fallbacks keep
            // the port running on devices that cannot expose a 4x MSAA surface.
            EGLConfig config = choose(egl, display, 4, 24, 8);
            if (config == null)
                config = choose(egl, display, 0, 24, 8);
            if (config == null)
                config = choose(egl, display, 0, 16, 0);
            if (config == null)
                throw new IllegalArgumentException("No compatible OpenGL ES 3 configuration");
            return config;
        }

        private EGLConfig choose(EGL10 egl, EGLDisplay display, int samples,
                                 int depthBits, int stencilBits) {
            ArrayList<Integer> attributes = new ArrayList<>();
            attributes.add(EGL10.EGL_RED_SIZE); attributes.add(8);
            attributes.add(EGL10.EGL_GREEN_SIZE); attributes.add(8);
            attributes.add(EGL10.EGL_BLUE_SIZE); attributes.add(8);
            attributes.add(EGL10.EGL_ALPHA_SIZE); attributes.add(8);
            attributes.add(EGL10.EGL_DEPTH_SIZE); attributes.add(depthBits);
            attributes.add(EGL10.EGL_STENCIL_SIZE); attributes.add(stencilBits);
            attributes.add(EGL10.EGL_RENDERABLE_TYPE); attributes.add(EGL_OPENGL_ES3_BIT_KHR);
            if (samples > 0) {
                attributes.add(EGL10.EGL_SAMPLE_BUFFERS); attributes.add(1);
                attributes.add(EGL10.EGL_SAMPLES); attributes.add(samples);
            }
            attributes.add(EGL10.EGL_NONE);

            int[] values = new int[attributes.size()];
            for (int i = 0; i < attributes.size(); ++i)
                values[i] = attributes.get(i);
            int[] count = new int[1];
            if (!egl.eglChooseConfig(display, values, null, 0, count) || count[0] <= 0)
                return null;
            EGLConfig[] configs = new EGLConfig[count[0]];
            if (!egl.eglChooseConfig(display, values, configs, configs.length, count))
                return null;
            return configs[0];
        }
    }

    private class Renderer implements GLSurfaceView.Renderer {
        private int inactiveTextInputFrames;

        public void onSurfaceCreated(GL10 gl, EGLConfig config) { }

        public void onSurfaceChanged(GL10 gl, int width, int height) {
            NativeLib.init(MainActivity.this.getAssets(), getFilesDir().getAbsolutePath(), width, height);
        }

        public void onDrawFrame(GL10 gl) {
            NativeLib.render();

            if (!keyboardVisible) {
                inactiveTextInputFrames = 0;
                return;
            }

            if (NativeLib.isTextInputActive()) {
                inactiveTextInputFrames = 0;
                return;
            }

            // Wait a few frames so an ordinary ID -> password focus switch does
            // not flicker Gboard. A real phase change keeps capture disabled.
            if (++inactiveTextInputFrames >= 3 && !keyboardHidePosted) {
                inactiveTextInputFrames = 0;
                keyboardHidePosted = true;
                runOnUiThread(() -> {
                    keyboardHidePosted = false;
                    if (!NativeLib.isTextInputActive())
                        hideKeyboard();
                });
            }
        }
    }

    private static final class ContentFile {
        final String path;
        final long size;
        final String sha256;

        ContentFile(String path, long size, String sha256) {
            this.path = path;
            this.size = size;
            this.sha256 = sha256.toLowerCase(Locale.ROOT);
        }
    }
}
