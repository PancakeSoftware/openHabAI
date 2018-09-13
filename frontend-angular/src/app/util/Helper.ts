import {ElementRef} from "@angular/core";

export class Tabs {
  constructor(private hostElement: ElementRef,
              private tabSelector: string,
              private attributeName: string) {
  }

  get active(): string {
    //return Array.from(this.hostElement.nativeElement.querySelectorAll('.dStructure-type li a'))
    //  .filter(el => el != {} && el.classList.contains('active'))
    //  .map(el => el.getAttribute(this.attributeName))[0];
    return 'currently not working :(';
  }

  set active(attributeValue: string) {

  }
}


export interface Point {
  x: number;
  y: number;
}

export function intersectsWithRect(pointToCheck: Point, rectUpLeft: Point, rectDownRight: Point) :boolean {
  return (rectUpLeft.x <= pointToCheck.x) && (pointToCheck.x <= rectDownRight.x) &&
         (rectUpLeft.y <= pointToCheck.y) && (pointToCheck.y <= rectDownRight.y);
}