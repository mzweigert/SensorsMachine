
function initWSClient(wsConnection) {
    ws = new WebSocket('ws://' + wsConnection.ip + ':' + wsConnection.port + '/sensors')
    ws.onopen = function (msg) {
        // Logic for opened connection
        console.log(msg);
    };
    ws.onmessage = function (msg) {
        initSensorsInfo(JSON.parse(msg.data));
    };

    ws.onclose = function (msg) {
        // Logic for closed connection
        console.log('Connection was closed.');
    }

    ws.error = function (err) {
        console.log(err); // Write errors to console
    }
}

function initSensorsInfo(sensorsState) {
    var $sensorsInfo = $('#sensors-info > tbody:last-child');
    sensorsState.forEach(state => {
        var sensor = $sensorsInfo.find("#sensor-" + state.id);
        if (sensor.length) {
            sensor.remove();
        }
        var $tr = $('<tr>');
        $tr.append('<td> Sensor ' + state.id + ' </td>')
            .append('<td> ' + state.soil_moisture + '% </td>')
            .attr('id', 'sensor-' + state.id)
            .appendTo($sensorsInfo);
    })
}

$(document).ready(function () {

    $.getJSON('/getWebSocketServerAddress', function (data) {
        initWSClient(data);
    });


    $.getJSON('/getSensorsState', function (data) {
        initSensorsInfo(data);
    });

});