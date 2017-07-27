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