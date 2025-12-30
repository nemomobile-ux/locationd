import QtQuick
import QtPositioning

Item{
    PositionSource {
        id: positionSource
        onPositionChanged: {
            console.log("Latitude"+" : "+positionSource.position.coordinate.latitude)
            console.log("Longitude"+" : "+positionSource.position.coordinate.longitude)
        }
        active: true
        updateInterval: 10

        Component.onCompleted: {
            console.log("Latitude"+" : "+positionSource.position.coordinate.latitude)
            console.log("Longitude"+" : "+positionSource.position.coordinate.longitude)
        }
    }
}
