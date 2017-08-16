let Navi = new function () {

    this.datastructureName = "";
    this.networkName ="";
    this.viewState = "datastructures";

    this.showDataStructureView = function () {
        this.viewState = "datastructures";
        $('.nav-network').addClass("no-display");
        $('#view-network').addClass("no-display");
        $('.nav-dataStructure').removeClass("no-display");
        $('#view-dataStructure').removeClass("no-display");

        $('#nav-at').find('.nav-at-structure, .nav-at-network, .nav-at-checkpoint').remove();

        // show
        DStructure.show();

        this.datastructureName = "";
        this.networkName ="";
        this.updateUrl();
    };

    this.showNetworksView = function (dataStructureId, NoTab) {
        if (this.viewState == "networks")
            return;

        this.viewState = "networks";
        $('.nav-network').removeClass("no-display");
        $('#view-network').removeClass("no-display");
        $('.nav-dataStructure').addClass("no-display");
        $('#view-dataStructure').addClass("no-display");

        if (NoTab == false || (typeof NoTab === 'undefined')) {
            $('.nav-network .tabs').tabs('select_tab', 'tab-networks');
        }

        // show
        DStructure.show();
        Networks.show(dataStructureId);

        this.datastructureName = dataStructureId;
        this.networkName ="";
        this.updateUrl();

        var el = $('#nav-at').find('.nav-at-structure');
        if (el.length) {
            el.text(dataStructureId);
            return;
        }


        $('#nav-at')
            .append('<a class="breadcrumb nav-at-structure">DataStructure_' + dataStructureId + '</a>');
    };

    this.showNetwork = function (dataStructureId, networkId) {
        this.showNetworksView(dataStructureId, true);
        $('.nav-network .tabs').tabs('select_tab', 'tab-train');

        this.datastructureName = dataStructureId;
        this.networkName = networkId;
        this.updateUrl();

        // show
        DStructure.show();
        Networks.show(dataStructureId);
        Network.show(dataStructureId, networkId);

        var el = $('#nav-at').find('.nav-at-network');
        if (el.length) {
            el.text(networkId);
            return;
        }

        $('#nav-at')
            .append('<a class="breadcrumb nav-at-network">'+networkId+'</a>');
    };


    // change url
    this.changeUrl = function (url) {
        history.replaceState({}, "", "#" + url);
    };

    this.updateUrl = function () {
        this.changeUrl(this.datastructureName + (this.networkName == "" ? "" : "/" + this.networkName))
    };

    // init all
    $(function () {
        $('#home').click(function () {
            Navi.showDataStructureView();
        });

        $('select').material_select();
        $('.modal').modal();


        // check what to show
        var url = window.location.hash;
        if (url.length <= 0 && (url.split('#')) <= 1) {
            Navi.showDataStructureView();
            return;
        }
        url = url.split('#')[1];

        toast("Go to: " + url, 5000);

        var urlParts = url.split('/');
        console.log(urlParts);

        if (urlParts.length == 1 || (urlParts[1] == "")) {
            // show networks of dataStructure
            console.info("show dataS " + urlParts[0]);
            Navi.showNetworksView(urlParts[0]);
            return;
        }

        if (urlParts.length == 2) {
            // show network
            console.info("show net " + urlParts[0] + "." + urlParts[1]);
            Navi.showNetwork(urlParts[0], urlParts[1]);
            return;
        }


        // if url is ??
        Navi.showDataStructureView();
    });
};