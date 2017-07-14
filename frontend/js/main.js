$(function() {
    // chart style
    Chart.defaults.global.defaultFontFamily = "'Roboto','Helvetica','Arial','sans-serif'";


    var chartLearnRate = new Chart($("#chart-learnRate"), {
        type: 'scatter',
        data: {
            datasets: [{
                data: [],
                label: "Error",
                borderColor: "#3e95cd",
                fill: false
            }, {
                data: [],
                label: "Test",
                borderColor: "#97e400",
                fill: false
            }
            ]
        },
        options: {
            legend: {
                display: false,
                position: 'right',
                labels: {
                    usePointStyle: true
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

    registerUpdatableChartGraph("progress", ["error", "test"], chartLearnRate);

    var i = 0;
    
    // buttons
    $('#bt-startTrain').click(function () {
        console.log("click");
        chartLearnRate.data.datasets[0].data.push({x:i,  y:Math.sin(i)});
        chartLearnRate.data.datasets[1].data.push({x:i,  y:Math.cos(i)});
        chartLearnRate.update();
        i+= 0.7;

        toast('Add x: ' + i +"  y: " + i*i);

    });


    // test network
    $('#bt-sendTxtTest').click(function () {
        console.log($('#txtSendTest').val());
        sockSend(JSON.parse($('#txtSendTest').val()));
    });

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
