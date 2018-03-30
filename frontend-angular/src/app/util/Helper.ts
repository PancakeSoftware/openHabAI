import {ElementRef} from "@angular/core";

export class Tabs {
  constructor(private hostElement: ElementRef,
              private tabSelector: string,
              private attributeName: string) {
  }

  get active(): string {
    return Array.from(this.hostElement.nativeElement.querySelectorAll('.dStructure-type li a'))
      .filter(el => el != {} && el.classList.contains('active'))
      .map(el => el.getAttribute(this.attributeName))[0]
  }

  set active(attributeValue: string) {

  }
}