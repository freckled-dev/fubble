import { Component, OnInit, OnDestroy } from '@angular/core';
import { LandingFixService } from '../../shared/services/landing-fix.service';
import {Meta, Title} from '@angular/platform-browser';
import {ActivatedRoute} from '@angular/router';

@Component({
  selector: 'app-request-form',
  templateUrl: './request-form.component.html',
  styleUrls: ['./request-form.component.scss']
})
export class RequestFormComponent implements OnInit, OnDestroy {

  constructor(
      private fix: LandingFixService,
      private route: ActivatedRoute,
      private title: Title,
      private meta: Meta
  ) { }

  ngOnInit() {
    this.fix.addFixPages();
    this.title.setTitle( this.route.snapshot.data['title']);
  }

  ngOnDestroy() {
    this.fix.removeFixPages();
  }

}
