package net.leezh.taleweaver;

import org.libsdl.app.SDLActivity;

public class MainActivity extends SDLActivity {
    public static native int nativeRunMain();

    @Override
    protected String[] getLibraries() {
        return new String[] {
            "SDL2",
            "taleweaver"
        };
    }
}