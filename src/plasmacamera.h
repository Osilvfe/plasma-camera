// SPDX-FileCopyrightText: 2025 Andrew Wang
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include <plasmacamera/settings.h>

#include "plasmacamera/worker.h"


/*
 * Camera interfaces with a libcamera camera
 * - directly controls the camera to produce a live feed
 * - switch to still image capture to take a picture
 * - gets settings and applies them
 */

/*
 * TODO: image rotation
 * TODO: Raw mode
 * TODO: video mode
 * TODO: stop and pause methods
 *
 * TODO: dealing with changing camera in settings
 *
 * TODO: instead of plasmacamera calling for updates in capabilities or ... worker should take initiative to report this stuff
 *		- when updating some value plasmacamera emits a signal for what it wants and worker emits a signal for what it did (along with another signal maybe for error)
 *		- as a result set does not actually set anything just trys to set
 *
 * TODO: either pass in a ControlList or a CameraConfig
 *		- the values must be already validated
 *		- just of question of if we want to CameraWorker to convert or Camera
 *		- CameraConfig can also just be key-value pair of a single thing to update
 *			- then only Camera stores a config list and we just update as things change
 *			- make a way to start CameraWorker with a vector of configs
 * TODO:
 * 	- camera landscape mode (lock when recording, rotate image/video)
 */

/**
 * \brief Controller object for the application camera state, used from QML.
 */
class PlasmaCamera : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool error READ error NOTIFY errorChanged)
    Q_PROPERTY(QString errorString READ errorString NOTIFY errorChanged)

    Q_PROPERTY(QList<QString> cameraDeviceIds READ cameraDeviceIds NOTIFY cameraDeviceListChanged)
    Q_PROPERTY(QList<QString> cameraDeviceNames READ cameraDeviceNames NOTIFY cameraDeviceListChanged)

    // Camera state
    Q_PROPERTY(bool available READ isAvailable NOTIFY availableChanged)
    Q_PROPERTY(bool busy READ busy NOTIFY busyChanged)
    Q_PROPERTY(QString cameraDevice READ cameraDevice WRITE setCameraDevice NOTIFY cameraDeviceChanged)
    Q_PROPERTY(QCameraFormat cameraFormat READ cameraFormat WRITE setCameraFormat NOTIFY cameraFormatChanged)

    // Focus
    // Q_PROPERTY(QSize afWindow READ afWindow WRITE setAfWindow RESET resetAfWindow NOTIFY afWindowChanged)

    // Brightness
    // Q_PROPERTY(int iso READ iso WRITE setIso RESET resetIso NOTIFY settingsChanged)
    // Q_PROPERTY(int exposureTime READ exposureTime WRITE setExposureTime RESET resetExposureTime NOTIFY settingsChanged)
    Q_PROPERTY(bool exposureValueAvailable READ exposureValueAvailable NOTIFY settingsChanged)
    Q_PROPERTY(float exposureValue READ exposureValue WRITE setExposureValue RESET resetExposureValue NOTIFY settingsChanged)

    // White balance
    Q_PROPERTY(bool whiteBalanceAvailable READ whiteBalanceAvailable NOTIFY settingsChanged)
    Q_PROPERTY(int whiteBalanceMode READ whiteBalanceMode WRITE setWhiteBalanceMode RESET resetAwb NOTIFY settingsChanged)
    // Q_PROPERTY(int wbTemp READ wbTemp WRITE setWbTemp RESET resetAwb NOTIFY settingsChanged)

    // Image processing
    // Q_PROPERTY(int contrast READ contrast WRITE setContrast RESET resetContrast NOTIFY settingsChanged)
    // Q_PROPERTY(int saturation READ saturation WRITE setSaturation RESET resetSaturation NOTIFY settingsChanged)
    Q_PROPERTY(bool noiseReductionAvailable READ noiseReductionAvailable NOTIFY settingsChanged)
    Q_PROPERTY(int noiseReductionMode READ noiseReductionMode WRITE setNoiseReductionMode NOTIFY settingsChanged)
    Q_PROPERTY(bool sharpnessAvailable READ sharpnessAvailable NOTIFY settingsChanged)
    Q_PROPERTY(float sharpness READ sharpness WRITE setSharpness NOTIFY settingsChanged)
    Q_PROPERTY(float minimumSharpness READ minimumSharpness NOTIFY settingsChanged)
    Q_PROPERTY(float maximumSharpness READ maximumSharpness NOTIFY settingsChanged)

    // Autofocus and flash
    Q_PROPERTY(bool autofocusAvailable READ autofocusAvailable NOTIFY settingsChanged)
    Q_PROPERTY(bool flashAvailable READ flashAvailable NOTIFY settingsChanged)
    Q_PROPERTY(int flashMode READ flashMode WRITE setFlashMode NOTIFY settingsChanged)

public:
    enum WhiteBalanceMode {
        WhiteBalanceAuto = 0,
        WhiteBalanceIncandescent = 1,
        WhiteBalanceFluorescent = 3,
        WhiteBalanceDaylight = 5,
        WhiteBalanceCloudy = 6,
    };
    Q_ENUM(WhiteBalanceMode)

    enum NoiseReductionMode {
        NoiseReductionOff = 0,
        NoiseReductionFast = 1,
        NoiseReductionHighQuality = 2,
    };
    Q_ENUM(NoiseReductionMode)

    enum FlashMode {
        FlashOff = 0,
        FlashSingle = 1,
        FlashTorch = 2,
    };
    Q_ENUM(FlashMode)

    explicit PlasmaCamera(QObject *parent = nullptr);
    ~PlasmaCamera() override;

    /*!
     * Returns whether there is a reported error from the camera backend.
     */
    bool error() const;

    /*!
     * Returns the error string of the reported error from the camera backend.
     */
    QString errorString() const;

    /*!
     * Switches the camera source to the next one.
     * Returns whether it was able to switch the camera successfully.
     */
    Q_INVOKABLE bool switchToNextCamera();

    /*!
     * Starts the camera thread if not already started. This starts the viewfinder
     * for generating previews.
     *
     * If the camera is not ready, it will wait until the state changes before starting.
     */
    Q_INVOKABLE void startCamera();

    /*!
     * Stops the camera thread if not already stopped.
     *
     * If the camera is not yet running, it will wait until the state changes before stopping.
     */
    Q_INVOKABLE void stopCamera();

    Q_INVOKABLE void triggerAutofocus();

    /*!
     * Returns all camera devices as a list of ids.
     */
    QList<QString> cameraDeviceIds() const;

    /*!
     * Returns all camera devices as a list of names.
     */
    QList<QString> cameraDeviceNames() const;

    /*!
     * Whether there is a camera running, and a stream available.
     */
    bool isAvailable() const;

    /*!
     * Whether the camera is busy (ex. taking a picture) and controls should be blocked.
     */
    bool busy() const;

    /*!
     * Returns the id of the current camera device.
     */
    QString cameraDevice() const;

    /*!
     * Returns the camera format of the current camera device.
     */
    QCameraFormat cameraFormat() const;

    /*!
     * Capture an image with the current camera.
     */
    bool captureImage();

    // Focus
    QSize afWindow() const;

    // Brightness
    int iso() const;
    int exposureTime() const;
    bool exposureValueAvailable() const;
    float exposureValue() const;

    // White balance
    bool whiteBalanceAvailable() const;
    int whiteBalanceMode() const;
    int wbTemp() const;

    // Contrast and saturation
    float contrast() const;
    float saturation() const;
    bool noiseReductionAvailable() const;
    int noiseReductionMode() const;
    bool sharpnessAvailable() const;
    float sharpness() const;
    float minimumSharpness() const;
    float maximumSharpness() const;

    bool autofocusAvailable() const;
    bool flashAvailable() const;
    int flashMode() const;

    // Camera settings
    Settings settings() const;
    float fps() const;
    int softwareRotationDegrees() const;
    bool mirrorOutput() const;

Q_SIGNALS:
    void errorChanged();
    void errorOccurred(const QString &errorString);
    void cameraDeviceListChanged();

    void availableChanged(bool available);
    void cameraDeviceChanged(const QString &cameraDevice);
    void cameraFormatChanged();
    void busyChanged();

    // Photos
    /*
     * While it would make the most sense to switch the libcamera config to StillCapture
     * for higher quality capture it requires restarting the camera which is simply too slow
     */
    void viewFinderFrame(const QImage &frame); // connect to this to get new frames
    void stillCaptureFrames(const QQueue<QImage> &frames); // connect to this to get captures

    // Focus
    void afWindowChanged(const QSize &newAfWindow);

    // Settings
    void settingsChanged(const Settings &settings);
    void fpsChanged(float fps);

public Q_SLOTS:
    // Camera state
    void setCameraDevice(const QString &cameraDeviceId);
    void setCameraFormat(const QCameraFormat &cameraFormat);

    // Focus
    void setAfWindow(const QSize &afWindow);
    void resetAfWindow();

    // Brightness
    void setIso(int iso);
    void setExposureTime(int exposureTime);
    void setExposureValue(float exposureValue);
    void resetIso();
    void resetExposureTime();
    void resetExposureValue();

    // White balance
    void setWhiteBalanceMode(int mode);
    void setWbTemp(int wbTemp);
    void resetAwb();

    // Contrast and saturation
    void setContrast(float contrast);
    void setSaturation(float saturation);
    void resetContrast();
    void resetSaturation();
    void setNoiseReductionMode(int mode);
    void setSharpness(float sharpness);
    void setFlashMode(int mode);

    void setFps(float fps);

private Q_SLOTS:
	void setError(const QString &errorString);
	void unsetError();

    void handleCameraAdded(std::shared_ptr<libcamera::Camera> camera);
    void handleCameraRemoved(std::shared_ptr<libcamera::Camera> camera);

private:
	enum class State
	{
        None, // Init
        Ready, // Libcamera manager and worker is started (set by worker)
        Running, // Ready for capture (set by worker)
        Stopping, // Shutting down
    };

    /*!
     * Starts the camera thread if not already started. This starts the viewfinder
     * for generating previews.
     */
    void startCameraInternal();

    /*!
     * Stops the camera thread if not already stopped.
     */
    void stopCameraInternal();

    /*!
     * Set the active state of the camera, and start/stop the camera as needed.
     */
    void setActive(bool active);

    /*!
     * Set the busy state of the camera.
     */
    void setBusy(bool busy);

    /*!
     * Set the internal state of the camera object.
     */
    void setState(State state);

    /*!
     * Attempt to acquire the current camera (from libcamera), and then fetch
     * camera details and load it into this object.
     *
     * Returns whether it was successful.
     */
    bool acquire();

    bool m_error = false;
    QString m_errorString;

    float m_fps = 30.0f;

    // The amount of degrees to rotate captured input by.
    int m_softwareRotationDegrees = 0;

    // Whether the camera output should be mirrored
    bool m_mirrorOutput = false;

    Settings m_settings;

    // Whether the camera has been requested to be active (see state for the actual camera state).
    bool m_active = false;

    // Whether the camera is in a task (ex. taking photo) and controls should be blocked
    bool m_busy = false;

    // The current state of the camera.
    State m_state = State::None;

    std::unique_ptr<libcamera::CameraManager> m_cameraManager;
    Worker *m_cameraWorker;
    QThread *m_cameraThread;

    QString m_cameraId;
    std::shared_ptr<libcamera::Camera> m_camera;

    QCameraFormat m_cameraFormat = QCameraFormat();
};
