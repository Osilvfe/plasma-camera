// SPDX-FileCopyrightText: 2025 Devin Lin <devin@kde.org>
// SPDX-License-Identifier: GPL-2.0-or-later

import QtQuick
import QtQuick.Controls as QQC2
import QtQuick.Layouts
import QtMultimedia

import org.kde.kirigami as Kirigami
import org.kde.plasmacamera
import org.kde.kirigamiaddons.components as Components

Rectangle {
    id: root

    required property bool audioRecordingEnabled
    required property bool audioRecordingEnabledShown
    required property bool exposureValueEnabled
    required property bool autofocusEnabled
    required property bool flashAvailable
    required property int flashMode

    signal audioEnabledChangeRequested(enabled: bool)
    signal exposureValueRequested(value: real)
    signal autofocusRequested()
    signal flashModeChangeRequested(mode: int)

    height: controlsLayout.implicitHeight
    color: Qt.rgba(0, 0, 0, 0.3)

    ColumnLayout {
        id: controlsLayout
        spacing: Kirigami.Units.smallSpacing
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.top: parent.top

        Kirigami.Theme.inherit: false
        Kirigami.Theme.colorSet: Kirigami.Theme.Complementary

        RowLayout {
            id: controlsRow

            spacing: Kirigami.Units.largeSpacing
            Layout.fillWidth: true
            Layout.topMargin: Kirigami.Units.largeSpacing
            Layout.bottomMargin: Kirigami.Units.largeSpacing

            Item { Layout.fillWidth: true }

            QQC2.ToolButton {
                id: audioRecordingButton
                icon.name: root.audioRecordingEnabled ? 'microphone-sensitivity-high-symbolic' : 'microphone-sensitivity-muted-symbolic'
                icon.color: 'white'
                text: root.audioRecordingEnabled ? i18n("Microphone is enabled") : i18n("Microphone is disabled")
                display: QQC2.ToolButton.IconOnly
                visible: root.audioRecordingEnabledShown
                onClicked: root.audioEnabledChangeRequested(!root.audioRecordingEnabled)

                QQC2.ToolTip.visible: down
                QQC2.ToolTip.text: text
            }

            QQC2.ToolButton {
                id: exposureControlsButton
                icon.name: 'lighttable' // TODO: find better icon
                icon.color: "white"
                text: i18n("Exposure")
                display: QQC2.ToolButton.IconOnly
                visible: root.exposureValueEnabled
                onClicked: exposureSelectStrip.shown = !exposureSelectStrip.shown

                QQC2.ToolTip.visible: down
                QQC2.ToolTip.text: text
            }

            QQC2.ToolButton {
                id: autofocusButton
                icon.name: "camera-focus"
                icon.color: "white"
                text: i18n("Autofocus")
                display: QQC2.ToolButton.IconOnly
                visible: root.autofocusEnabled
                onClicked: root.autofocusRequested()

                QQC2.ToolTip.visible: down
                QQC2.ToolTip.text: text
            }

            QQC2.ToolButton {
                id: flashButton
                icon.source: root.flashMode === 0 ? "qrc:/camera_flash_off.png" : "qrc:/camera_flash_fill.png"
                text: root.flashMode === 0 ? i18n("Flash off")
                    : root.flashMode === 1 ? i18n("Flash on capture")
                    : i18n("Torch")
                display: QQC2.ToolButton.IconOnly
                visible: root.flashAvailable
                onClicked: root.flashModeChangeRequested((root.flashMode + 1) % 3)

                QQC2.ToolTip.visible: down
                QQC2.ToolTip.text: text
            }

            Item { Layout.fillWidth: true }
        }

        ExposureSelectStrip {
            id: exposureSelectStrip
            visible: root.exposureValueEnabled
            shown: false // Set by exposureControlsButton

            onExposureValueRequested: (value) => root.exposureValueRequested(value)
            Layout.fillWidth: true
        }
    }
}
