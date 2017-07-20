// -- charts ------------------------------------------------------------
var charts = [];

function registerUpdatableChartGraph(chartName, graphNames, chart) {
    charts[chartName] = [graphNames, chart];
}

function pushToChartGraph(data) {
    console.log(charts);

    var chart = charts[data.chart];
    if (chart == undefined)
        return;


    for (var g in data.graphs) {
        var at = chart[0].indexOf(data.graphs[g].graph);
        if (at < 0)
            continue;

        if (data.type == "add") {
            chart[1].data.datasets[at].data = chart[1].data.datasets[at].data.concat(data.graphs[g].data);
        }
        else {
            chart[1].data.datasets[at].data = data.graphs[g].data;
        }

        console.log(JSON.stringify(data.graphs[g].data, null, 2));

        //console.log(data.graphs[g].data);
    }
    chart[1].update();
}


$(function () {

    // -- settings ------------------------------------------------------------
    $('#tab-settings #bt-save').click(function () {

        var data = {
            action: 'update',
            what: 'settings',
            expectRespond: true,
            data: []
        };
        $('#tab-settings').find('input').each(function (at) {
            data.data.push({
                property: $(this).attr('id'),
                value: $(this).val()
            });
        });

        sockSend(data);

        toastOk("Settings saved");
    });

    // test network
    $('#bt-sendTxtTest').click(function () {
        //sockSend(JSON.parse($('#txtSendTest').val()));
        sock.onmessage({
            data: $('#txtSendTest').val()
        })
    });


    // buttons -- train
    $('#bt-startTrain').click(function () {
        sockSend("do", "startTrain",
            function (status) {
                if (status == "ok")
                    toastOk('start train');
            });
    });

    $('#bt-stopTrain').click(function () {
        sockSend("do", "stopTrain",
            function (status) {
                if (status == "ok")
                    toastOk('training stopped');
            });
    });

});

// -- connection ------------------------------------------------------------
var sock = new WebSocket('ws://localhost:5555');
sock.onerror = function (error) {
    toastErr("can't connect to server");
};

sock.onopen = function (p1) {
    toastOk("connected with server");
};

// message id
var respondCounter = 0;
var respondCallbacks = [];

sock.onmessage = function (event) {
    //console.log(event.data);
    var data = JSON.parse(event.data);

    networkDebugAppend("GET --------------", data);

    switch (data.type) {
        case "updateChart":
            pushToChartGraph(data.what);
            break;

        case "respond":
            respondCallbacks[data.respondId](data.what);
            delete respondCallbacks[data.respondId];
            break;

        case "message":
            toast(data.what.message);
            break;

        default:
            toastErr("got unknown message type '" + data.type + "'");
    }
};

function sockSend(type, what) {
    var data = {
        "type": type,
        "what": what
    }

    sock.send(JSON.stringify(data));
    networkDebugAppend("SEND -------------", data);
}

function sockSend(type, what, callback) {
    var data = {
        "type": type,
        "respondId": respondCounter,
        "what": what
    }

    respondCallbacks[respondCounter] = callback;
    respondCounter++;

    sock.send(JSON.stringify(data));
    networkDebugAppend("SEND -------------", data);
}
