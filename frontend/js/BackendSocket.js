// -- charts ------------------------------------------------------------
var Charts = new function () {
    this.charts = [];

    this.registerUpdatableChartGraph = function(chartName, graphNames, chart) {
        this.charts[chartName] = [graphNames, chart];
    };

    this.pushToChartGraph = function(data) {
        console.log(this.charts);

        var chart = this.charts[data.chart];
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
    };
};


// -- socket ------------------------------------------------------------
var Sock = new function () {

    // -- connection ------------------------------------------------------------
    this.sock = new WebSocket('ws://localhost:5555');
    this.sock.onerror = function (error) {
        toastErr("can't connect to server");
    };

    this.sock.onopen = function (p1) {
        toastOk("connected with server");
    };

    // message id
    this.respondCounter = 0;
    this.respondCallbacks = [];

    this.sock.onmessage = function (event) {
        //console.log(event.data);
        var data = JSON.parse(event.data);

        networkDebugAppend("GET --------------", data);

        switch (data.type) {
            case "updateChart":
                Charts.pushToChartGraph(data.what);
                break;

            case "respond":
                if (data.data !== undefined)
                    Sock.respondCallbacks[data.respondId](data.what, data.data);
                else
                    Sock.respondCallbacks[data.respondId](data.what);
                delete Sock.respondCallbacks[data.respondId];
                break;

            case "message":
                toast(data.what.message);
                break;

            default:
                toastErr("got unknown message type '" + data.type + "'");
        }
    };


    this.send = function sockSend(type, what, callback, data) {
        var dataS = {
            "type": type,
            "what": what
        };

        if ((data !== undefined) && (!$.isEmptyObject(data)))
            dataS.data = data;

        if (callback !== undefined) {
            // want respond
            dataS.respondId = this.respondCounter;
            this.respondCallbacks[this.respondCounter] = callback;
            this.respondCounter++;
        }

        networkDebugAppend("SEND -------------", dataS);
        this.sock.send(JSON.stringify(dataS));
    };

    // -- init all ---------------
    $(function () {
        // test
        $('#bt-sendTxtTest').click(function () {
            //sockSend(JSON.parse($('#txtSendTest').val()));
            Sock.sock.onmessage({
                data: $('#txtSendTest').val()
            })
        });
    });
};


