import {Component, OnInit} from '@angular/core';
import {Api} from "../util/Api/Api";
import {ActivatedRoute} from "@angular/router";
import {toastInfo, toastOk} from "../util/Log";
import {FormBuilder, FormGroup} from "@angular/forms";
import {ApiObject} from "../util/Api/ApiObject";

@Component({
  selector: 'app-network-train',
  templateUrl: './network-train.component.html',
  styles: []
})
export class NetworkTrainComponent implements OnInit {

  structureID: number = -1;
  networkID: number = -1;
  trainParams: FormGroup;
  private network: ApiObject<any>;

  constructor(private router: ActivatedRoute,
              private fb: FormBuilder,
              public api: Api)
  {


    // get route changes
    router.params.subscribe(params => {
      if (this.structureID !== params.structureID || this.networkID !== params.networkID) {
        this.structureID = params.structureID;
        this.networkID = params.networkID;
        this.updateSelf();
      }

      // setup form
      this.trainParams = fb.group({
          optimizer: "sgd",
          learnRate: 12
        }
      );
    });
  }


  ngOnInit() {
  }

  private updateSelf() {
    toastInfo("update ... " + this.structureID + "/" + this.networkID);

    // store self
    this.network = this.api.object(`/dataStructures/${this.structureID}/networks/${this.networkID}/`);


  }

  onTriggerTrain(startTrain: boolean) {
    if (startTrain) {
      toastInfo("start train: ", this.trainParams.value)
      this.network.update(this.trainParams.value);
      this.network.action('startTrain')
        .then(() => toastOk('start train'));;
    }
    else
      this.network.action('stopTrain').then(() => toastOk('stopped train'));
  }
}
