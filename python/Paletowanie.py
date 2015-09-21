from Manipulator3D import *

listOfTargets = ['Target', 'Target.001', 'Target.002', 'Target.003', 'Target.004', 'Target.005', 'Target.006']
centerOfTargets = vec4(0,0,0,1)

delAction = CommandReturnAction.DelAndForward

def nullFun( RC, scene, dt):
    print '[ NullFun ]'
    return True

def goOverTargetAndGrab(RC, position, offset, target, idx):
    q = quatRev(getScene().get(target).quat)
    RC.goTo(delAction).to(position + offset).insert(RC, idx)
    # RC.goTo(delAction).to(position + offset).orientation(angleAxis(0, vec3(0,0,-1))).insert(RC, idx+1)
    RC.goTo(delAction).to(position + offset).orientation(q).insert(RC, idx+1)
    # RC.goTo(delAction).to(position).orientation(angleAxis(0, vec3(0,0,-1))).insert(RC, idx+2)
    RC.goTo(delAction).to(position).orientation(q).insert(RC, idx+2)
    RC.grab(getScene().get(target), delAction).insert(RC, idx+3)
    RC.goTo(delAction).to(position + offset).name('1').insert(RC, idx+4)
    return idx+5

def goOverTargetAndRelease(RC, position, offset, idx):
    RC.goTo(delAction).to(position + offset).insert(RC, idx)
    RC.goTo(delAction).to(position + offset).orientation(angleAxis(3.1415, vec3(0,0,-1))).insert(RC, idx+1)

    RC.goTo(delAction).to(position).orientation(angleAxis(3.1415, vec3(0,0,-1))).insert(RC, idx+2)

    RC.release(delAction).insert(RC, idx+3)
    RC.goTo(delAction).to(position + offset).name('1').insert(RC, idx+4)
    return idx+5

def moveFromTo(RC, pFrom, pTo, target):
    print '[ Hahahaha ]'
    idx = goOverTargetAndGrab(RC, pFrom, vec4(0,0,1,0), target, 1)
    idx = goOverTargetAndRelease(RC, pTo, vec4(0,0,1,0), idx)
    RC.pyExec(CommandReturnAction.DelAndBack).name('3214').fun(nullFun).insert(RC, idx)

class Paletter:
    def __init__(self, scene,  target, targets):
        self.startCenter = self.computeCenterPosition(scene, targets)
        self.targets = targets
        self.target = target
        self.ready = True
        self.idx = 0

    def computeCenterPosition(self, scene, targets):
        out = vec4(0,0,0,0)
        for target in targets:
            print target
            pos = scene.get(target).position
            out = out + pos
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
        self.idx += 1
        return out

    def onUpdate(self, RC, scene, dt):
        if self.ready and len(self.targets) > self.idx:
            self.ready = False
            entName = self.nextTarget()
            ent = scene.get(entName)

            positionOffset = ent.position - self.startCenter
            to = self.target + positionOffset

            moveFromTo(RC, ent.position, to, entName)
            return True
        elif len(self.targets) == self.idx:
            return True
        return False

def update(RC, scene, dt):
	pass

def init(RC, scene):
    print '[ Paletowanie ]'
    paletter = Paletter(scene, vec4(2,-2,2,1), listOfTargets)
    RC.pyExec(1).name('Paletting').callback(paletter).finish(RC)

def terminate(RC, scene):
	pass

def handleInput(key, action, mod, RC, scene):
    pass
