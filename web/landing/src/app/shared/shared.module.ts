import { CommonModule } from '@angular/common';
import { NgModule } from '@angular/core';
import { RouterModule } from "@angular/router";
import { TranslateModule } from '@ngx-translate/core';
import { NgxPageScrollModule } from 'ngx-page-scroll';
import { LandingFixService } from '../shared/services/landing-fix.service';
import { FooterComponent } from './footer/footer.component';
import { HeaderComponent } from './header/header.component';
import { WINDOW_PROVIDERS } from "./services/windows.service";



@NgModule({
  exports: [
    CommonModule,
    HeaderComponent,
    FooterComponent,
    TranslateModule
  ],
  imports: [
    CommonModule,
    RouterModule,
    NgxPageScrollModule,
    TranslateModule
  ],
  declarations: [
    HeaderComponent,
    FooterComponent
  ],
  providers: [
    WINDOW_PROVIDERS,
    LandingFixService
  ]
})
export class SharedModule { }
