
function initWSClient(wsConnection) {
    ws = new WebSocket('ws://' + wsConnection.ip + ':' + wsConnection.port + '/machine')
    ws.onopen = function (msg) {
        // Logic for opened connection
        console.log(msg);
    };
    ws.onmessage = function (msg) {
        initMachineInfo(JSON.parse(msg.data));
    };

    ws.onclose = function (msg) {
        // Logic for closed connection
        console.log('Connection was closed.');
    }

    ws.error = function (err) {
        console.log(err); // Write errors to console
    }
}

function initMachineInfo(machineState) {
    var $machineInfo = $('#machine-info > tbody:last-child');
    $machineInfo.empty();
    var sensors = new Map(machineState.sensors.map(entry => [entry.id, entry.soil_moisture]));
    var pumps = new Map(machineState.pumps.map(entry => [entry.number, entry.state]));
    sensors.forEach((soil_moisture, id) => {
        var sensor = $machineInfo.find("#sensor-" + id);
        if (sensor.length) {
            sensor.remove();
        }
        var $tr = $('<tr>');
        var $button = generateButton(id, soil_moisture, pumps.get(id));
        var $tdBtn = $("<td>");
        $tdBtn.append($button);
        $tr.append('<td> Sensor ' + id + ' </td>')
            .append('<td> ' + soil_moisture + '% </td>')
            .append($tdBtn)
            .attr('id', 'sensor-' + id)
            .attr('sensor-id', id)
            .appendTo($machineInfo);
    })
}

function generateButton(id, soil_moisture, pump_state) {
    var $btn = $('<button class="btn btn-primary"> Water </button>');
    if (soil_moisture >= 75 || pump_state != 'IDLE') {
        $btn.attr("disabled", true);
    } else {
        $btn.click(function (e) {
            $btn.attr("disabled", true);
            const payload = {
                number: id
            }
            $.post("/runPump", JSON.stringify(payload)).fail(function () {
                $btn.removeAttr("disabled");
            });

        });
    }
    return $btn;
}

$(document).ready(function () {

    $.getJSON('/getWebSocketServerAddress', function (data) {
        initWSClient(data);
    });


    $.getJSON('/getMachineInfo', function (data) {
        initMachineInfo(data);
    });

});