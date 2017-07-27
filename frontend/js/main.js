// -- LOG ---------------------------------
function toast(msg, time) {
    if (time != undefined)
        Materialize.toast(msg, time);
    else
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
