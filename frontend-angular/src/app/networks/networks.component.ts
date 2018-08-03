import {Component, Input, OnInit} from '@angular/core';
import {ActivatedRoute, Params} from "@angular/router";
import {Observable} from "rxjs/Observable";
import {toastInfo, toastOk} from "../util/Log";
import {Api} from "@catflow/Api";
import {ApiList} from "@catflow/ApiList";
import {FormBuilder, FormGroup} from "@angular/forms";
import {BehaviorSubject} from "rxjs/BehaviorSubject";

@Component({
  selector: 'app-networks',
  templateUrl: './networks.component.html',
  styles: []
})
export class NetworksComponent implements OnInit
{
  structureID: number = -1;
  formNew: FormGroup;

  networks: Observable<Network[]>;
  private networksList: ApiList<Network>;
  private dataStructure: BehaviorSubject<any>;

  constructor(
    fb: FormBuilder,
    private router: ActivatedRoute,
    private api: Api)
  {
    // get route changes
    router.params.subscribe(params => {
      if (this.structureID !== params.structureID) {
        this.structureID = params.structureID;
        this.updateSelf();
      }
    });

    // form new
    this.formNew = fb.group({
      name: 'NeuralNetwork',
      hidden: 2,
      neuronsPerHidden: 10
    });

    // init dropdown
    $('#train-optimizer').dropdown();
  }

  ngOnInit() {
  }

  /* reload all
   */
  updateSelf() {
    // get data
    this.dataStructure = this.api.object(`/dataStructures/${this.structureID}`).object();
    this.networksList = this.api.list(`/dataStructures/${this.structureID}/networks`);
    this.networks =this.networksList.items();
  }

  removeNet(net: Network) {
    this.networksList.remove(net.id);
  }

  createNetwork() {
    toastInfo('new net', this.formNew.value);
    this.networksList.add(this.formNew.value);
    this.formNew.reset({
      name: 'NeuralNetwork',
      hidden: 2,
      neuronsPerHidden: 10
    });
  }

  getNamesOf(item: any) {
    return item.map(el => el[1] + ' ');
  }

  getInputnameById(id: number) {
    return this.dataStructure.getValue().inputNames.filter(i => i[0] == id)[0][1];
  }
}

type Network = {name: string, id: number};