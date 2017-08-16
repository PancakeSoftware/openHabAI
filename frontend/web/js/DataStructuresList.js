var DStructure = new function () {

    this.loaded = false;

    this.show = function () {

        // load if not already
        if (this.loaded)
            return;

        this.loaded = true;
        this.clear();

        // load list
        Sock.send("get", "datastructures", function (what, data) {
            $.each(data, function (key, item) {
                DStructure.addStructure(
                    item.name,
                    item.id,
                    item.type);
            });
        });
    };

    this.addStructure = function (name, id, type) {
        if (name === "") {
            name = "?";
        }
        let n = $(`        
        <div class="card" datastructure=` + id + `>
            <div class="card-content">
                <i class="material-icons left medium">memory</i>
                <div class="card-title">` + name + `</div>
                <p><i class="material-icons left blue-grey-text">` + ((type === "function") ? "functions" : "home") + `</i>` + ((type === "function") ? "Function" : "OpenHab") + `</p>
            </div>
         </div>`);

        n.click(DStructure.onNetworkEntryClick);
        $('#tab-dataStructure').append(n)
    };

    this.clear = function () {
        $('#tab-dataStructure').empty();
    };



    this.onNetworkEntryClick = function () {
        let d = $(this).attr("datastructure");
        toast("ID: " + d);
        Networks.show(d);
        Navi.showNetworksView(d);
    };


    // init all
    $(function () {

        // modal -- new dataStructure
        $('.modal').modal();
        var chartFunction = new Chart($("#chart-function"), {
            type: 'scatter',
            data: {
                datasets: [{
                    label: "f(x)",
                    borderColor: "#3e95cd",
                    backgroundColor: "#3e95cd",
                    fill: false
                }]
            },
            options: {
                legend: {
                    display: false
                }
            }
        });

        $('#txt-func').on('input', updateFunc);
        $('#num-from').on('input', updateFunc);
        $('#num-to').on('input', updateFunc);

        function updateFunc() {
            try {
                var funcP = math.parse($('#txt-func').val());
                var func = funcP.compile();
                $(this).css("color", "");

                chartFunction.data.datasets[0].data = [];
                var max = parseInt($('#num-to').val());
                var x = parseInt($('#num-from').val());
                var steps = 60;
                if ((max - x) <= 0)
                    return;
                var step = (max - x) / steps;
                for (; x <= max; x += step) {
                    chartFunction.data.datasets[0].data = chartFunction.data.datasets[0].data.concat({
                        x: x,
                        y: func.eval({x: x})
                    })
                }
                chartFunction.update();

            } catch (e) {
                $(this).css("color", "red");
            }
        }

        updateFunc();

        // create new
        $('#bt-new-DataStructure').click(function () {
            toastInfo("new Data Structure");

            let send = {
                name: $('#txt-name').val(),
                type: $('.dStructure-type .active').attr('type'),
                function: $('#txt-func').val(),
                range: {
                    from: $('#num-from').val(),
                    to:   $('#num-to').val()
                }
            };

            Sock.send("create", "datastructure", function (what, data) {
                if (what !== "ok") {
                    toastErr("cant create datastructure");
                    return;
                }

                // add
                DStructure.addStructure(data.name, data.dataStructureId, data.type);
            }, send);
        })
    })
};
