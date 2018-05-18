import {Component, EventEmitter, OnInit} from '@angular/core';
import {ActivatedRoute, Params} from "@angular/router";

@Component({
  selector: 'app-networks-and-train',
  templateUrl: './networks-and-train.component.html',
  styles: []
})
export class NetworksAndTrainComponent implements OnInit {

  networkID: number;
  structureID: number;
  tabsElement: any;

  constructor(private route: ActivatedRoute) {
    route.params.subscribe(params => {
      this.networkID = params['networkID'];
      this.structureID = params['structureID'];

      // show train tab
      if (this.networkID != undefined && (this.tabsElement != undefined)) {
        console.info('select tab', this.tabsElement);
        this.tabsElement.tabs('select_tab', 'tab-run');
      }
    });
  }

  ngOnInit() {
    // reload the tabs of materializz
    $('ul.tabs').tabs();
    this.tabsElement = $('.nav-network .tabs');

    // show train tab
    if (this.networkID != undefined) {
      console.info('select tab', this.tabsElement);
      this.tabsElement.tabs('select_tab', 'tab-run');
    }
  }

}
