import {$} from 'protractor';
import {MaterializeAction, MaterializeDirective, MaterializeModule} from "angular2-materialize";

declare var Materialize: any;


export function toast(msg, time) {
  if (time != undefined) {
    Materialize.toast(msg, time);
  } else {
    Materialize.toast(msg, 1500);
  }
}


export function toastOk(msg) {
  Materialize.toast('<i class="material-icons left circle green">done</i>' + msg, 1500);
}

export function toastInfo(msg) {
  Materialize.toast('<i class="material-icons left circle blue">info_outline</i>' + msg, 1500);
}

export function toastErr(msg) {
  Materialize.toast('<i class="material-icons left circle red">error_outline</i>' + msg, 2000);
}

