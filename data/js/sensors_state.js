
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
    var sensors = new Map(machineState.sensors.map(entry => [entry.id, entry.soil_moisture]));
    var pumps = new Map(machineState.pumps.map(entry => [entry.number, entry.state]));
    sensors.forEach((soil_moisture, id) => {
        var $tr = $machineInfo.find("#sensor-" + id);
        if ($tr.length) {
            $tr.find('td:lt(2)').remove();
        } else {
            $tr = $('<tr>')
                .attr('id', 'sensor-' + id)
                .attr('sensor-id', id);
            $tr.appendTo($machineInfo);
        }

        $tr.prepend('<td> ' + soil_moisture + '% </td>')
            .prepend('<td> Sensor ' + id + ' </td>');

        updatePumpController($tr, id, soil_moisture, pumps);
    })
}

function updatePumpController($tr, id, soil_moisture, pumps) {
    var inputsCreated = false;
    var $breakTime = $('#break-time-' + id);
    if (!$breakTime.length) {
        $breakTime = $('<input id=\"break-time-' + id + '\" type="number" value="10" min="10" max="30">');
        inputsCreated = true;
    }
    var $wateringTime = $('#watering-time-' + id);
    if (!$wateringTime.length) {
        $wateringTime = $('<input id=\"watering-time-' + id + '\" type="number" value="5" min="5" max="20">');
        inputsCreated = true;
    }
    var $button = $('#run-pump-btn-' + id);
    if (!$button.length) {
        $button = generateButton(id, soil_moisture, pumps.get(id), $breakTime, $wateringTime);
        inputsCreated = true;
    }
    if (!inputsCreated) {
        if (soil_moisture >= 75 || pumps.get(id) != 'IDLE') {
            $button.attr("disabled", true);
            $breakTime.attr("disabled", true);
            $wateringTime.attr("disabled", true);
        } else {
            $button.removeAttr("disabled");
            $breakTime.removeAttr("disabled");
            $wateringTime.removeAttr("disabled");
        }
    } else if (inputsCreated) {
        $tr.append(createTableData($button))
            .append(createTableData($breakTime))
            .append(createTableData($wateringTime));

        $breakTime.inputSpinner();
        $wateringTime.inputSpinner();
    }
}
function createTableData($child) {
    var $tdBtn = $("<td>");
    $tdBtn.append($child);
    return $tdBtn;
}

function generateButton(id, soil_moisture, pump_state, $breakTimeInput, $wateringTimeInput) {
    var $btn = $('<button id="run-pump-btn-' + id + '" class="btn btn-primary"> Water </button>');
    if (soil_moisture >= 75 || pump_state != 'IDLE') {
        $btn.attr("disabled", true);
        $breakTimeInput.attr("disabled", true);
        $wateringTimeInput.attr("disabled", true);
    } else {
        $btn.click(function (e) {
            $btn.attr("disabled", true);
            $breakTimeInput.attr("disabled", true);
            $wateringTimeInput.attr("disabled", true);

            const payload = {
                number: id,
                break_time: $breakTimeInput.val(),
                watering_time: $wateringTimeInput.val()
            }
            $.post("/runPump", JSON.stringify(payload)).fail(function () {
                $btn.removeAttr("disabled");
                $breakTimeInput.removeAttr("disabled");
                $wateringTimeInput.removeAttr("disabled");
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