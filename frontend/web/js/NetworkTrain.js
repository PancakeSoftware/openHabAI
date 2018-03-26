var Network = new function () {

    this.networkId = -1;
    this.datastructure = -1;

    this.myRoute = function () {
        return [
            {dataStructures: String(this.datastructure)},
            {networks: String(this.networkId)}
        ];
    };

    this.show = function (dataStructure, networkId) {

        // load if not already
        if (this.dataStructure === dataStructure && this.networkId === networkId) {
            return;
        }

        // unSubscribe old updates
        //Sock.send("unsubscribe", "updateChart",null, this.jsonMe());

        // clear
        this.clear();

        this.networkId = networkId;
        this.datastructure = dataStructure;

        // load contend
        Sock.send(this.myRoute(), "get", function (what, data) {
            // set all fields
            $('#train-learnRate').val(data.learnRate);
            $('#train-optimizer').val(data.optimizer).material_select();
            $('#train-optimizer').material_select();
            toastInfo("optimizer '" + data.optimizer +"'");
            // reinit select

            toastOk("load network " + data.name);
        });


        // register chart updates
        Sock.send(this.myRoute().concat([ {"charts": ""}, {"progress": ""}]), "subscribe");
        Sock.send(this.myRoute().concat([ {"charts": ""}, {"progress": ""}]), "update", null, {
            fixedInputs: [1,2,4],
            rangeInputs: [
                {from: 10, to: 22, id: 0},
                {from: 99, to: 99, id: 1}
            ],
            outputs: [5,2,40]
        });
        //Sock.send("subscribe", "updateChart", null, this.jsonMe());

    };

    this.clear = function () {
        // clear charts
        Network.chartLearnRate.data.datasets[0].data = [];
        Network.chartLearnRate.data.datasets[1].data = [];
        Network.chartOutputShape.data.datasets[0].data = [];
        Network.chartOutputShape.data.datasets[1].data = [];
        Network.chartLearnRate.update();
        Network.chartOutputShape.update();
    };

    // -- init all ------------------------------------
    $(function() {
        // chart style
        Chart.defaults.global.defaultFontFamily = "'Roboto','Helvetica','Arial','sans-serif'";
        Chart.defaults.global.legend.position = "right";


        Network.chartLearnRate = new Chart($("#chart-learnRate"), {
            type: 'scatter',
            data: {
                datasets: [{
                    data: [],
                    label: "Error",
                    backgroundColor: "#3e95cd",
                    borderColor: "#3e95cd",
                    fill: false
                }, {
                    data: [],
                    label: "Test",
                    backgroundColor: "#97e400",
                    borderColor: "#97e400",
                    fill: false
                }
                ]
            },
            options: {
                legend: {
                    display: true,
                    position: 'right',
                    labels: {
                        usePointStyle: false
                    }

                },
                scales: {
                    xAxes: [{
                        type: 'linear',
                        position: 'bottom'
                    }],
                    yAxes: [{
                        type: 'linear',
                        position: 'left'
                    }]
                }
            }
        });

        Network.chartOutputShape= new Chart($("#chart-outputShape"), {
            type: 'scatter',
            data: {
                datasets: [{
                    label: "Network",
                    borderColor: "#3e95cd",
                    backgroundColor: "#3e95cd",
                    fill: false
                }, {
                    label: "Real",
                    borderColor: "#97e400",
                    backgroundColor: "#97e400",
                    fill: false
                }
                ]
            }
        });

        Charts.registerUpdatableChartGraph("progress", ["error", "test"], Network.chartLearnRate);
        Charts.registerUpdatableChartGraph("outputShape", ["network", "real"], Network.chartOutputShape);


        // ----------------------------
        // buttons -- train
        $('#bt-startTrain').click(function () {
            // update values
            Sock.send(Network.myRoute(), "update", null, {
                optimizer: $('#train-optimizer').val(),
                learnRate: Number($('#train-learnRate').val())
            });

            // start train
            Sock.send(Network.myRoute(), "do",
                function (status) {
                    if (status == "ok")
                        toastOk('start train');
                }, {"do": "startTrain"});
        });

        $('#bt-stopTrain').click(function () {
            // stop train
            Sock.send(Network.myRoute(), "do",
                function (status) {
                    if (status == "ok")
                        toastOk('stop train');
                }, {"do": "stopTrain"});
        });
    });
};