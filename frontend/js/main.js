$(function() {
    // chart style
    Chart.defaults.global.defaultFontFamily = "'Roboto','Helvetica','Arial','sans-serif'";
    Chart.defaults.global.legend.position = "right";


    var chartLearnRate = new Chart($("#chart-learnRate"), {
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
                },

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

    var chartOutputShape= new Chart($("#chart-outputShape"), {
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

    registerUpdatableChartGraph("progress", ["error", "test"], chartLearnRate);
    registerUpdatableChartGraph("outputShape", ["network", "real"], chartOutputShape);

    var i = 0;
    




    $.ajax({url: "jsonProtocol/updateChart.json", success: function(result){
        console.log(result);
        $('#txtSendTest').val(JSON.stringify(result, null, 2));
        $('#txtSendTest').trigger('autoresize');
    }});


});


// -- LOG ---------------------------------
function toast(msg) {
    Materialize.toast(msg, 1500);
}


function toastOk(msg) {
    Materialize.toast('<i class="material-icons left circle green">done</i>' + msg, 1500);
}

function toastInfo(msg) {
    Materialize.toast('<i class="material-icons left circle blue">info_outline</i>' + msg, 1500);
}

function toastErr(msg) {
    Materialize.toast('<i class="material-icons left circle red">error_outline</i>' + msg, 2500);
}

function networkDebugAppend(msg, json) {
    $('#debug-network-console').append("<br><h6>"+ msg +"</h6>" +
        Prism.highlight(JSON.stringify(json, null, 2), Prism.languages.json)
    );
    $('#debug-network-console').animate({scrollTop: $('#debug-network-console')[0].scrollHeight}, 'fast');
}
