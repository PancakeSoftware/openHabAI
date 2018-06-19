import {Component, Input, OnInit} from '@angular/core';
import {ApiObject} from "@catflow/ApiObject";
import {selector} from "rxjs/operator/publish";
import {Api} from "@catflow/Api";

@Component({
  selector: 'app-selection-view',
  templateUrl: './selection-view.component.html',
  styles: []
})
export class SelectionViewComponent implements OnInit {

  _optionObject: any;
  @Input() key:string;
  selectedOption:any = 0;

  constructor(public api:Api) { }

  ngOnInit() {
  }

  @Input() set optionObject(opOb: any)
  {
      for (let i in opOb.optionValues)
          if (opOb.optionValues[i] == true)
              this.selectedOption = i;

      this._optionObject = opOb;
  }

  get optionObject() {
      return this._optionObject;
  }

  onChange(i:number) {
    for (let i in this.optionObject.optionValues)
        this.optionObject.optionValues[i] = false;
    console.log('changed ', this.optionObject);
    this.optionObject.optionValues[i] = true;

    this.api.object("/settings/").update({
      [this.key]:{
        optionValues:this.optionObject.optionValues
      }
    });
  }

}
