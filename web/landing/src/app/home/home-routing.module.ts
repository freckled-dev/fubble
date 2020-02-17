import { NgModule } from '@angular/core';
import { Routes, RouterModule } from '@angular/router';

import { HomeComponent } from './home.component';

// Routes
const routes: Routes = [
  {
    path: 'one',
    component: HomeComponent,
    data: {
      title: "Home | fubble Angular 8 Landing Page",
      content: "fubble Angular 8 landing page with Angular Universal | SSR | SEO friendly"
    }
  }
];

@NgModule({
  imports: [RouterModule.forChild(routes)],
  exports: [RouterModule]
})
export class HomeRoutingModule { }
