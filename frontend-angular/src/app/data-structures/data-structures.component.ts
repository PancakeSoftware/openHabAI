import {Component, ElementRef, EventEmitter, OnDestroy, OnInit, ViewChild, ViewChildren} from '@angular/core';
import {MaterializeAction} from "angular2-materialize";
import {ApiConnection} from "../util/Api/ApiConnection";
import {FormBuilder, FormGroup} from "@angular/forms";
import {toastErr, toastInfo, toastOk} from "../util/Log";
import {Observable} from "rxjs/Observable";
import {el} from "@angular/platform-browser/testing/src/browser_util";
import {Tabs} from "../util/Helper";
import {Api} from "../util/Api/Api";
import {ApiList} from "../util/Api/ApiList";


@Component({
  selector: 'app-data-structures',
  templateUrl: './data-structures.component.html',
  styles: []
})
export class DataStructuresComponent implements OnInit, OnDestroy {

  modalActions = new EventEmitter<string|MaterializeAction>();


  dataStructuresList: ApiList<DataStructure>;
  dataStructures: Observable<DataStructure[]>;

  formNew: FormGroup;
  formNewTypeTabs: Tabs;


  /* setup all
   */
  constructor(
    fb: FormBuilder,
    hostElement: ElementRef,
    private api: Api)
  {
    // get data
    this.dataStructuresList = api.list<DataStructure>("/dataStructures");
    this.dataStructures = this.dataStructuresList.items();


    // control tabs
    this.formNewTypeTabs = new Tabs(hostElement, '.dStructure-type','type');

    // new datastruture form
    this.formNew = fb.group({
      name: 'MyDataStructure',
      type: '',
      function: 'x^3 + y^2',
      inputNames: 'x y',
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
    this.formNew.value.type = "function";//this.formNewTypeTabs.active;
    this.formNew.value.inputNames = this.formNew.value.inputNames.split(' ');
    this.formNew.value.outputNames = ['function'];
    //toastInfo('New DataStructure: ', this.formNew.value);

    this.dataStructuresList.add(this.formNew.value)
      .then(value => toastOk('Added dataStructure (id: ' + value + ')'))
      .catch(reason => toastErr("can't add dataStructure: " + reason));

    this.formNew.reset({
      name: 'MyDataStructure',
      type: '',
      function: 'x^3 + y^2',
      inputNames: 'x y',
      range_from: -10,
      range_to: 10
    });
  }

  removeStruc(struc: DataStructure) {
    this.dataStructuresList.remove(struc.id);
  }

  ngOnDestroy(): void {
    console.log('dataStructures destroy!');
  }
}



type DataStructure = {name: string, id: number, type: string};