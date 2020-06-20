function toHHMMSS(timestring) {
    var sec_num = parseInt(timestring, 10)
    var hours = Math.floor(sec_num / 3600)
    var minutes = Math.floor((sec_num - (hours * 3600)) / 60)
    var seconds = sec_num - (hours * 3600) - (minutes * 60)

    if (hours < 10) {
        hours = "0" + hours
    }
    if (minutes < 10) {
        minutes = "0" + minutes
    }
    if (seconds < 10) {
        seconds = "0" + seconds
    }

    if (hours < 1) {
        return minutes + ':' + seconds
    }

    return hours + ':' + minutes + ':' + seconds
}

function getRandomColor() {
    var letters = '0123456789ABCDEF'
    var color = '#'
    for (var i = 0; i < 6; i++) {
        color += letters[Math.floor(Math.random() * 16)]
    }
    return color
}

function isDemoMode() {
    for (var i = 0; i < Qt.application.arguments.length; i++) {
        var argument = Qt.application.arguments[i]
        if (argument === "-demoMode") {
            return true
        }
    }

    return false
}
