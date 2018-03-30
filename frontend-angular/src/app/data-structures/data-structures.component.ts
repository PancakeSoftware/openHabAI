import {Component, ElementRef, EventEmitter, OnInit, ViewChild, ViewChildren} from '@angular/core';
import {MaterializeAction} from "angular2-materialize";
import {Backend} from "../util/backendSocket";
import {FormBuilder, FormGroup} from "@angular/forms";
import {toastInfo} from "../util/Log";
import {Observable} from "rxjs/Observable";
import {el} from "@angular/platform-browser/testing/src/browser_util";
import {Tabs} from "../util/Helper";


@Component({
  selector: 'app-data-structures',
  templateUrl: './data-structures.component.html',
  styles: []
})
export class DataStructuresComponent implements OnInit {

  modalActions = new EventEmitter<string|MaterializeAction>();

  dataStructures:
      {name: string, id: number, type: string}[] = [];

  formNew: FormGroup;
  formNewTypeTabs: Tabs;


  /* setup all
   */
  constructor(fb: FormBuilder, hostElement: ElementRef) {
    // control tabs
    this.formNewTypeTabs = new Tabs(hostElement, '.dStructure-type','type');

    // new datastruture form
    this.formNew = fb.group({
      name: 'MyDataStructure',
      type: '',
      function: 'x^3',
      range_from: -10,
      range_to: 10
    });

    // get data
    Backend.sendRequest([{"dataStructures": ""}], "getAll", (what, data) => {
      if (what == 'ok')
        this.dataStructures = data;  //data.map(item => this.dataStructures.push(item));
    });
  }

  ngOnInit() {
    toastInfo('reinit structs');
  }

  /* new dataStructure
   */
  createNew() {
    // set type by tabs
    this.formNew.value.type = this.formNewTypeTabs.active;

    Backend.sendRequest([{"dataStructures": ""}], "add", (what, data) => {
      if (what == 'ok')
        this.dataStructures.push(data);
    }, this.formNew.value);
  }

  removeStruc(struc) {
    toastInfo('remove is not implemented yet :(');
    Backend.sendRequest([{"dataStructures": struc.id.toString()}], "remove", (what) => {
      if (what != 'ok')
        return;
      let index = this.dataStructures.findIndex(el => el.id == struc.id);
      if (index > -1)
        this.dataStructures.splice(index, 1);
    });
  }
}