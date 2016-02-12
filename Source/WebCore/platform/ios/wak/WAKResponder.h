/*
 * Copyright (C) 2005, 2006, 2007, 2008, 2009 Apple Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE INC. AND ITS CONTRIBUTORS ``AS IS''
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL APPLE INC. OR ITS CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
 * THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef WAKResponder_h
#define WAKResponder_h

#if TARGET_OS_IPHONE

#import "WKTypes.h"
#import <Foundation/Foundation.h>

@class WebEvent;

@interface WAKResponder : NSObject
{

}

- (void)handleEvent:(WebEvent *)event;

- (void)scrollWheel:(WebEvent *)theEvent;
- (BOOL)tryToPerform:(SEL)anAction with:(id)anObject;
- (void)mouseEntered:(WebEvent *)theEvent;
- (void)mouseExited:(WebEvent *)theEvent;
- (void)keyDown:(WebEvent *)event;
- (void)keyUp:(WebEvent *)event;
#if ENABLE(TOUCH_EVENTS)
- (void)touch:(WebEvent *)event;
#endif

- (void)insertText:(NSString *)text;

- (void)deleteBackward:(id)sender;
- (void)deleteForward:(id)sender;
- (void)insertParagraphSeparator:(id)sender;

- (void)moveDown:(id)sender;
- (void)moveDownAndModifySelection:(id)sender;
- (void)moveLeft:(id)sender;
- (void)moveLeftAndModifySelection:(id)sender;
- (void)moveRight:(id)sender;
- (void)moveRightAndModifySelection:(id)sender;
- (void)moveUp:(id)sender;
- (void)moveUpAndModifySelection:(id)sender;

- (WAKResponder *)nextResponder;
- (BOOL)acceptsFirstResponder;
- (BOOL)becomeFirstResponder;
- (BOOL)resignFirstResponder;

- (void)mouseUp:(WebEvent *)theEvent;
- (void)mouseDown:(WebEvent *)theEvent;
- (void)mouseMoved:(WebEvent *)theEvent;

@end

#endif // TARGET_OS_IPHONE

#endif // WAKResponder_h
