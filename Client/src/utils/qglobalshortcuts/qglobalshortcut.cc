/**
* This file is part of https://github.com/mitei/qglobalshortcut
* Code is provided as copy/paste solution
* Kudos to mitei :)
*/

#include "qglobalshortcut.h"
#include <QCoreApplication>
#include <QKeySequence>
#include <Windows.h>

QMultiHash<quint32, QGlobalShortcut*> QGlobalShortcut::shortcuts_;
QGlobalShortcut::QGlobalShortcutEventFilter QGlobalShortcut::global_shortcut_event_;

QGlobalShortcut::QGlobalShortcut(QObject* parent)
    : QObject(parent)
{
    initialize();
}

QGlobalShortcut::QGlobalShortcut(const QKeySequence &keyseq, QObject *parent)
    : QObject(parent)
{
    initialize();
    setKey(keyseq);
}

void QGlobalShortcut::initialize() {
    static bool initialized = false;
    if (!initialized) {
        qApp->installNativeEventFilter(&global_shortcut_event_);
        initialized = true;
    }
}

QGlobalShortcut::~QGlobalShortcut() {
    unsetKey();
}

QKeySequence QGlobalShortcut::key() const
{
    return keyseq_;
}

void QGlobalShortcut::setKey(const QKeySequence& keyseq)
{
    if (!keyseq_.isEmpty()) {
        unsetKey();
    }
    quint32 keyid = calcId(keyseq);
    if (shortcuts_.count(keyid) == 0) {
        quint32 keycode = toNativeKeycode(getKey(keyseq));
        quint32 mods = toNativeModifiers(getMods(keyseq));
        registerKey(keycode, mods, keyid);
    }
    this->keyseq_ = keyseq;
    shortcuts_.insert(keyid, this);
}

void QGlobalShortcut::unsetKey() {
    quint32 keyid = calcId(keyseq_);
    if (shortcuts_.remove(keyid, this) > 0) {
        if (shortcuts_.count(keyid) == 0) {
            quint32 keycode = toNativeKeycode(getKey(keyseq_));
            quint32 mods = toNativeModifiers(getMods(keyseq_));
            unregisterKey(keycode, mods, keyid);
        }
    }
}

bool QGlobalShortcut::activate(quint32 id) {
    if (shortcuts_.contains(id)) {
        foreach (QGlobalShortcut* s, shortcuts_.values(id)) {
            emit s->activated();
        }
        return true;
    }
    return false;
}

quint32 QGlobalShortcut::calcId(const QKeySequence& keyseq) {
    quint32 keycode = toNativeKeycode(getKey(keyseq));
    quint32 mods    = toNativeModifiers(getMods(keyseq));
    return calcId(keycode, mods);
}

#ifndef Q_OS_UNIX
quint32 QGlobalShortcut::calcId(quint32 k, quint32 m) {
    return k | m;
}
#endif

Qt::Key QGlobalShortcut::getKey(const QKeySequence& keyseq) {
    if (keyseq.isEmpty()) {
        return Qt::Key(0);
    }
    return Qt::Key(keyseq[0] & ~Qt::KeyboardModifierMask);
}

Qt::KeyboardModifiers QGlobalShortcut::getMods(const QKeySequence& keyseq) {
    if (keyseq.isEmpty()) {
        return Qt::KeyboardModifiers(0);
    }
    return Qt::KeyboardModifiers(keyseq[0] & Qt::KeyboardModifierMask);
}



bool QGlobalShortcut::QGlobalShortcutEventFilter::nativeEventFilter(const QByteArray& event_type,
    void* message, long* result)
{
    MSG* msg = static_cast<MSG*>(message);
    if (msg->message == WM_HOTKEY) {
        quint32 id = static_cast<quint32>(msg->wParam);
        activate(id);
        return true;
    }
    return false;
}

quint32 QGlobalShortcut::toNativeKeycode(Qt::Key k) {
    // F1 ~ F24
    if (k >= Qt::Key_F1 && k <= Qt::Key_F24)
        return VK_F1 + (k - Qt::Key_F1);
    // 0 ~ 9
    if (k >= Qt::Key_0 && k <= Qt::Key_9)
        return k;
    // A ~ Z
    if (k >= Qt::Key_A && k <= Qt::Key_Z)
        return k;

    switch (k) {
    case Qt::Key_Escape:
        return VK_ESCAPE;
    case Qt::Key_Tab:
    case Qt::Key_Backtab:
        return VK_TAB;
    case Qt::Key_Backspace:
        return VK_BACK;
    case Qt::Key_Return:
    case Qt::Key_Enter:
        return VK_RETURN;
    case Qt::Key_Insert:
        return VK_INSERT;
    case Qt::Key_Delete:
        return VK_DELETE;
    case Qt::Key_Pause:
        return VK_PAUSE;
    case Qt::Key_Print:
        return VK_PRINT;
    case Qt::Key_Clear:
        return VK_CLEAR;
    case Qt::Key_Home:
        return VK_HOME;
    case Qt::Key_End:
        return VK_END;
    case Qt::Key_Left:
        return VK_LEFT;
    case Qt::Key_Up:
        return VK_UP;
    case Qt::Key_Right:
        return VK_RIGHT;
    case Qt::Key_Down:
        return VK_DOWN;
    case Qt::Key_PageUp:
        return VK_PRIOR;
    case Qt::Key_PageDown:
        return VK_NEXT;
    case Qt::Key_Space:
        return VK_SPACE;
    case Qt::Key_Asterisk:
        return VK_MULTIPLY;
    case Qt::Key_Plus:
        return VK_ADD;
    case Qt::Key_Comma:
        return VK_SEPARATOR;
    case Qt::Key_Minus:
        return VK_SUBTRACT;
    case Qt::Key_Slash:
        return VK_DIVIDE;
    case Qt::Key_Backslash:
        return VK_OEM_102;        // ?
    case Qt::Key_MediaNext:
        return VK_MEDIA_NEXT_TRACK;
    case Qt::Key_MediaPrevious:
        return VK_MEDIA_PREV_TRACK;
    case Qt::Key_MediaPlay:
        return VK_MEDIA_PLAY_PAUSE;
    case Qt::Key_MediaStop:
        return VK_MEDIA_STOP;
        //case Qt::Key_MediaLast:     // ??
        //case Qt::Key_MediaRecord:   // ??
    case Qt::Key_VolumeDown:
        return VK_VOLUME_DOWN;
    case Qt::Key_VolumeUp:
        return VK_VOLUME_UP;
    case Qt::Key_VolumeMute:
        return VK_VOLUME_MUTE;
    default:
        return 0;
    }
}

quint32 QGlobalShortcut::toNativeModifiers(Qt::KeyboardModifiers m) {
    quint32 mods = 0;
    if (m & Qt::AltModifier)      mods |= MOD_ALT;
    if (m & Qt::ControlModifier)  mods |= MOD_CONTROL;
    if (m & Qt::ShiftModifier)    mods |= MOD_SHIFT;
    if (m & Qt::MetaModifier)     mods |= MOD_WIN;
    return mods;
}

void QGlobalShortcut::registerKey(quint32 k, quint32 m, quint32 id) {
    RegisterHotKey(NULL, id, m, k);
}

void QGlobalShortcut::unregisterKey(quint32 k, quint32 m, quint32 id) {
    UnregisterHotKey(NULL, id);
}
