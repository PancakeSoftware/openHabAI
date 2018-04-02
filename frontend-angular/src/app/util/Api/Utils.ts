export class  ApiRouteContainig {
  readonly route: ApiRoute;

  constructor(route: ApiRoute) {
    this.route = route
  }
}


export type ApiRoute = string;