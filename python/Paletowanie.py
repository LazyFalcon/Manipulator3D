from Manipulator3D import *
from math import floor

listOfTargets = ['Target.007', 'Target.001', 'Target.002', 'Target.003', 'Target.004', 'Target.005', 'Target.006', 'Target', 'Target.008']
centerOfTargets = vec4(0,0,0,1)

delAction = CommandReturnAction.DelAndForward

def nullFun( RC, scene, dt):
    print '[ NullFun ]'
    return True

def goOverTargetAndGrab(RC, position, offset, target, idx):
    eq = quatRev(getScene().get(target).quat)
    q = fromAxes(eq*vec4(0,0,-1,0), eq*vec4(1,0,0,0))
    RC.goTo(delAction).to(position + offset).insert(RC, idx)
    RC.goTo(delAction).to(position + offset).orientation(q).insert(RC, idx+1)
    RC.goTo(delAction).to(position).orientation(q).insert(RC, idx+2)
    RC.grab(getScene().get(target), delAction).insert(RC, idx+3)
    RC.goTo(delAction).to(position + offset).name('1').insert(RC, idx+4)
    return idx+5

def goOverTargetAndRelease(RC, position, offset, orientation, idx):
    RC.goTo(delAction).to(position + offset).insert(RC, idx)
    # RC.goTo(delAction).to(position + offset).orientation(angleAxis(1.0707, vec3(0,0,-1))).insert(RC, idx+1)
    RC.goTo(delAction).to(position + offset).orientation(orientation).insert(RC, idx+1)

    RC.goTo(delAction).to(position).orientation(orientation).insert(RC, idx+2)

    RC.release(delAction).insert(RC, idx+3)
    RC.goTo(delAction).to(position + offset).name('1').insert(RC, idx+4)
    return idx+5

def moveFromTo(RC, pFrom, pTo, target):
    idx = goOverTargetAndGrab(RC, pFrom, vec4(0,0,1,0), target, 1)

    # orientation = fromAxis(vec3(0,0,-1))
    orientation = fromAxes(vec3(0,0,-1), vec3(1,0,0))

    idx = goOverTargetAndRelease(RC, pTo, vec4(0,0,1,0), orientation, idx)
    RC.pyExec(CommandReturnAction.DelAndBack).fun(nullFun).insert(RC, idx)

class Paletter:
    def __init__(self, scene,  target, targets, dims):
        self.startCenter = self.computeCenterPosition(scene, targets)
        self.targets = targets
        self.target = target
        self.ready = True
        self.dims = dims
        self.idx = 0
        self.count = len(targets)

    def computeCenterPosition(self, scene, targets):
        out = vec4(0,0,0,0)
        x = vec4(0,1,0,0)
        for target in targets:
            print target
            pos = scene.get(target).position
            out = out + pos
            print scene.get(target).quat * x
        out = out/out.w
        return out

    def onEnter(self, RC):
        self.ready = True
        print '[ Hello ]'
        pass

    def onExit(self, RC, scene):
        pass

    def nextTarget(self):
        out = self.targets[self.idx]
        return out

    def nextInPalette(self):
        x = self.idx % self.dims[0]
        y = floor(self.idx / self.dims[1])
        return self.target + vec4(x, y, 0, 0)*vec4(0.5, 0.9,0,0)

    def onUpdate(self, RC, scene, dt):
        if self.ready and len(self.targets) > self.idx:
            self.ready = False
            entName = self.nextTarget()
            ent = scene.get(entName)

            positionOffset = ent.position - self.startCenter
            # to = self.target + positionOffset
            to = self.nextInPalette()

            moveFromTo(RC, ent.position, to, entName)
            self.idx += 1
            return True
        elif len(self.targets) == self.idx:
            return True
        return False

def update(RC, scene, dt):
	pass

def init(RC, scene):
    print '[ Paletowanie ]'
    paletter = Paletter(scene, vec4(2,-2,2,1), listOfTargets, (3,3))
    RC.pyExec(1).name('Paletting').callback(paletter).finish(RC)

def terminate(RC, scene):
	pass

def handleInput(key, action, mod, RC, scene):
    pass
