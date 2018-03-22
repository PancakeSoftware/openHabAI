var Networks = new function ()
{
    // var
    this.dataStructure = -1;

    this.show = function (dataStructure) {

        // load if not already
        if (this.dataStructure === dataStructure)
            return;

        this.dataStructure = dataStructure;
        this.clear();

        // load list
        Sock.send([{"dataStructures": dataStructure}, {"networks": ""}], "getAll", function (what, data) {
            $.each(data, function (key, item) {
                Networks.addNetwork(
                    item.name,
                    item.id);
            });
        });
    };

    this.addNetwork = function (name, id) {
        if (name === "")
            name = "?";

        let n = $(`        
        <div class="card" network=`+id+`>
            <div class="card-content">
                <i class="material-icons left medium">timeline</i>
                <div class="card-title">` + name + `</div>
                <p>ID: ` + id + `</p>
            </div>
         </div>`);
        n.click(Networks.onNetworkEntryClick);
        $('#tab-networks').find('.networks').append(n)
    };

    this.clear = function () {
        $('#tab-networks').find('.networks').empty();
    };

    this.onNetworkEntryClick = function () {
        toast("will load network " + $(this).attr("network"));
        Navi.showNetwork(Networks.dataStructure, $(this).attr("network"));
    };


    // init all
    $(function () {

        // button new net
        $('#bt-new-network').click(function ()
        {
            let send = {
                datastructure: Networks.dataStructure,
                name: $('#txt-name-net').val(),
                hidden: Number($('#num-hidden').val()),
                neuronsPerHidden: Number($('#num-neronsPerHidden').val())
            };

            Sock.send([{"dataStructures": Networks.dataStructure}, {"networks": ""}], "add", function (what, data) {
                if (what !== "ok") {
                    toastErr("cant create network");
                    return;
                }

                // add
                Networks.addNetwork(data.name, data.id);
            }, send);

            // reset all
            $('#txt-name-net').val('');
            $('#num-neronsperhidden').val(10);
            $('#num-hidden').val(2);
        })
    });

};