import {Component, ElementRef, EventEmitter, OnDestroy, OnInit, ViewChild, ViewChildren} from '@angular/core';
import {MaterializeAction} from "angular2-materialize";
import {ApiConnection} from "../util/Api/ApiConnection";
import {FormBuilder, FormGroup} from "@angular/forms";
import {toastInfo} from "../util/Log";
import {Observable} from "rxjs/Observable";
import {el} from "@angular/platform-browser/testing/src/browser_util";
import {Tabs} from "../util/Helper";
import {Api} from "../util/Api/Api";


@Component({
  selector: 'app-data-structures',
  templateUrl: './data-structures.component.html',
  styles: []
})
export class DataStructuresComponent implements OnInit, OnDestroy {

  modalActions = new EventEmitter<string|MaterializeAction>();


  dataStructures: Observable<DataStructure[]>;

  formNew: FormGroup;
  formNewTypeTabs: Tabs;


  /* setup all
   */
  constructor(
    fb: FormBuilder,
    hostElement: ElementRef,
    api: Api)
  {
    // get data
    let list = api.list<DataStructure>("/dataStructures");
    this.dataStructures = list.items();


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
    /*
    ApiConnection.sendRequest([{"dataStructures": ""}], "getAll", (what, data) => {
      if (what == 'ok')
        this.dataStructures = data;  //data.map(item => this.dataStructures.push(item));
    });
    */
  }

  ngOnInit() {
  }



  /* new dataStructure
   */
  createNew() {
    // set type by tabs
    this.formNew.value.type = this.formNewTypeTabs.active;
    toastInfo('New DataStructure: ', this.formNew.value);

    ApiConnection.sendRequest("/dataStructures", "add", (what, data) => {
      //if (what == 'ok')
      //  this.dataStructures.push(data);
    }, this.formNew.value);

    this.formNew.reset();
  }

  removeStruc(struc) {
    toastInfo('remove is not implemented yet :(');
    ApiConnection.sendRequest(`dataStructures/${struc.id.toString()}`, "remove", (what) => {
      if (what != 'ok')
        return;
      //let index = this.dataStructures.findIndex(el => el.id == struc.id);
      //if (index > -1)
      //  this.dataStructures.splice(index, 1);
    });
  }

  ngOnDestroy(): void {
    console.log('dataStructures destroy!');
  }
}



type DataStructure = {name: string, id: number, type: string};