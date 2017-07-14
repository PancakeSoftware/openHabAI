
// -- charts ------------------------------------------------------------
var charts = [];

function registerUpdatableChartGraph(chartName, graphNames, chart) {
    charts[chartName] =  [graphNames, chart];
}

function pushToChartGraph(data) {
    console.log(charts);

    var chart = charts[data.chart];
    if (chart == undefined)
        return;

    for (var c in data.data) {
        var at = chart[0].indexOf(data.data[c].graph);
        if (at < 0)
            return false;

        chart[1].data.datasets[at].data.push({x: data.data[c].x, y:data.data[c].y} );
        console.log({x: data.data[c].x, y:data.data[c].y});
    }
    chart[1].update();
}


// -- settings ------------------------------------------------------------
$(function() {
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
    })
});



// -- connection ------------------------------------------------------------
var sock = new WebSocket('ws://html5rocks.websocket.org/echo');
sock.onerror = function (error) {
    toastErr("can't connect to server");
};

sock.onopen = function (p1) {
    toastOk("connected with server");
};

sock.onmessage = function (event) {
    console.log(event.data);
    var data = JSON.parse(event.data);

    networkDebugAppend("GET --------------", data);


    /* Example JSON:
     {
     "chart": "progress",
     "x": 10,
     "data": [
     {
     "name": "error",
     "x": 5
     },
     {
     "name": "test",
     "x": 8
     }
     ]
     }


     *
     */
    pushToChartGraph(data)
};

function sockSend(msg) {
    sock.send(JSON.stringify(msg));
    networkDebugAppend("SEND -------------", msg);
}