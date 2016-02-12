/*
 * Copyright (C) Research In Motion Limited 2010. All rights reserved.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#ifndef SVGAnimatedNumberOptionalNumber_h
#define SVGAnimatedNumberOptionalNumber_h

#if ENABLE(SVG)
#include "SVGAnimatedTypeAnimator.h"

namespace WebCore {

class SVGAnimationElement;

class SVGAnimatedNumberOptionalNumberAnimator FINAL : public SVGAnimatedTypeAnimator {
public:
    SVGAnimatedNumberOptionalNumberAnimator(SVGAnimationElement*, SVGElement*);

    virtual std::unique_ptr<SVGAnimatedType> constructFromString(const String&) OVERRIDE;
    virtual std::unique_ptr<SVGAnimatedType> startAnimValAnimation(const SVGElementAnimatedPropertyList&) OVERRIDE;
    virtual void stopAnimValAnimation(const SVGElementAnimatedPropertyList&) OVERRIDE;
    virtual void resetAnimValToBaseVal(const SVGElementAnimatedPropertyList&, SVGAnimatedType*) OVERRIDE;
    virtual void animValWillChange(const SVGElementAnimatedPropertyList&) OVERRIDE;
    virtual void animValDidChange(const SVGElementAnimatedPropertyList&) OVERRIDE;

    virtual void addAnimatedTypes(SVGAnimatedType*, SVGAnimatedType*) OVERRIDE;
    virtual void calculateAnimatedValue(float percentage, unsigned repeatCount, SVGAnimatedType*, SVGAnimatedType*, SVGAnimatedType*, SVGAnimatedType*) OVERRIDE;
    virtual float calculateDistance(const String& fromString, const String& toString) OVERRIDE;
};

} // namespace WebCore

#endif // ENABLE(SVG)
#endif
