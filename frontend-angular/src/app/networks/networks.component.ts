import {Component, Input, OnInit} from '@angular/core';
import {ActivatedRoute, Params} from "@angular/router";
import {Observable} from "rxjs/Observable";
import {toastInfo, toastOk} from "../util/Log";
import {ApiConnection} from "../util/Api/ApiConnection";
import {Api} from "../util/Api/Api";
import {ApiList} from "../util/Api/ApiList";
import {FormBuilder, FormGroup} from "@angular/forms";

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
}

type Network = {name: string, id: number};