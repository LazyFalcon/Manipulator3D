
bl_info = {
		"name":         "Scene exporter",
		"author":       "DezerteR",
		"blender":      (2,6,7),
		"version":      (0,0,1),
		"location":     "File > Export > Scene exporter",
		"description":  "Export Scene",
		"category":     "Import-Export"
}

import bpy
import mathutils
import string
import os
from bpy_extras.io_utils import ExportHelper

def tstr(number):
	# return str(round(number, 6))
	return str("%.4f" %number)

def vecToString(vec):
	str = '['+tstr(vec.x)+', '+tstr(vec.y)+', '+tstr(vec.z)+']'
	return str

def getVectTo(a, b):
	Pa = a.matrix_world*mathutils.Vector((0.0, 0.0, 0.0, 1.0))
	Pb = b.matrix_world*mathutils.Vector((0.0, 0.0, 0.0, 1.0))
	return Pb - Pa

def writePosition(file, object, offset):
	position = object.matrix_world*mathutils.Vector((0.0, 0.0, 0.0, 1.0))
	file.write(offset + '  Position: ' + vecToString(position))
	return position
def writeQuaternion(file, object, offset):
	# quat = object.rotation_euler.to_quaternion()
	object.rotation_mode = 'QUATERNION'
	quat = object.rotation_quaternion
	str = '['+tstr(quat[0])+', '+tstr(quat[1])+', '+tstr(quat[2])+', '+tstr(quat[3])+']'
	file.write(offset + '  Quaternion: ' + str)
	return quat
def writeMaterial(file, object, offset):
	# write color
	mat = object.active_material
	if mat:
		color = mat.diffuse_color
		specta = tstr(mat.specular_intensity)
		str = '['+tstr(color[0])+', '+tstr(color[1])+', '+tstr(color[2])+']'
		file.write(offset + '  Color: ' + str)
		file.write(offset + '  Spectacular: ' + specta)
	else:
		file.write(offset + '  Color: [0.1473, 0.1473, 0.1473]')
		file.write(offset + '  Spectacular: 1.0')
def writeRigidBodyProperties(file, object, offset):
	rgBody = object.rigid_body
	if(rgBody):
		o = offset + '    '
		file.write(offset + '  RigidBody: ')


		file.write(o + 'enabled: ' + "yes")
		file.write(o + 'type: ' + rgBody.type)
		file.write(o + 'collision_shape: ' + rgBody.collision_shape)
		file.write(o + 'mass: ' + tstr(rgBody.mass))
		file.write(o + 'friction: ' + tstr(rgBody.friction))

		points = []
		for b in object.bound_box:
			for x in b:
				points.append(tstr(x))

		arr = " ".join(points)
		file.write(o + 'BBox: ' + arr)

def writeLampColor(file, object, offset):
	color = object.color
	str = '['+tstr(color[0])+', '+tstr(color[1])+', '+tstr(color[2])+']'
	file.write(offset + '  Color: ' + str)

def writeScene(file, objects, offset, dirname, sceneName):
	file.write(offset + 'Meshes:')
	offset = offset + ' '
	for obj in objects:
		if obj.type == 'MESH':
			file.write(offset + '- Name: ' + obj.name)

			# remember and write position
			currentLoc = writePosition(file, obj, offset)
			currentQuat = writeQuaternion(file, obj, offset)
			writeMaterial(file, obj, offset)
			writeRigidBodyProperties(file, obj, offset)
			# select object
			bpy.ops.object.select_pattern(pattern = obj.name, extend = False)
			# bpy.ops.transform.select_orientation(orientation='GLOBAL')
			# move to 000
			bpy.ops.transform.translate(value = -currentLoc.xyz)
			aa = currentQuat.to_axis_angle()
			bpy.ops.transform.rotate(value = -aa[1], axis = aa[0])

			bpy.ops.export_scene.obj(check_existing=False, filepath=dirname+'\\'+sceneName+'\\'+obj.name+'.obj', axis_forward='Y', axis_up='Z', filter_glob="*.obj;*.mtl", use_selection=True, use_animation=False, use_mesh_modifiers=True, use_edges=True, use_smooth_groups=False, use_smooth_groups_bitflags=False, use_normals=True, use_uvs=True, use_materials=False, use_triangles=True, use_nurbs=False, use_vertex_groups=False, use_blen_objects=True, group_by_object=False, group_by_material=False, keep_vertex_order=False, global_scale=1.0, path_mode='AUTO')

			bpy.ops.transform.rotate(value = aa[1], axis = aa[0])
			bpy.ops.transform.translate(value = currentLoc.xyz)

			# bpy.ops.uv.unwrap()

def writeLamps(file, objects, offset):
	file.write(offset + 'Lamps:')
	offset = offset + ' '
	for object in objects:
		if object.type == 'LAMP':
			file.write(offset + '- Name: ' + object.name)
			# object = bpy.data.objects[lamp.name]

			lamp = bpy.data.lamps[object.data.name]
			writePosition(file, object, offset)
			writeQuaternion(file, object, offset)
			writeLampColor(file, lamp, offset)

			file.write(offset + '  Falloff_distance: ' + tstr(lamp.distance))
			file.write(offset + '  Energy: ' + tstr(lamp.energy))
			file.write(offset + '  Type: ' + lamp.type)

			# if lamp.type == 'POINT':
				# file.write(offset + '  falloff_type: ' + lamp.falloff_type)

			if lamp.type == 'SPOT':
				# file.write(offset + '  falloff_type: ' + lamp.falloff_type)
				file.write(offset + '  spot_size: ' + tstr(lamp.spot_size))

			if lamp.type == 'AREA':
				file.write(offset + '  Size: ' + tstr(lamp.size))

def writeProperties(file, obj, offset):
	for K in obj.keys():
		if K not in '_RNA_UI':
			file.write(offset+K+': ')
			file.write('%s\n' % obj[K])

def writeRobot(file, mesh):
	file.write('\n - Name: '+mesh.name)

	parent = mesh.parent
	axis = parent.matrix_local*mathutils.Vector((0.0, 0.0, 1.0, 0.0))
	axis.normalize()
	# file.write('\n   Type: '+parent["Type"])
	file.write('\n')
	writeProperties(file, parent,'   ')
	file.write('   ParentJoint: ')
	file.write('\n     Name: '+parent.name)
	file.write('\n     Vec: '+vecToString(getVectTo(parent, mesh)))
	file.write('\n     Axis: '+vecToString(axis))
	# file.write('\n     Min: '+parent['Min'])
	# file.write('\n     Max: '+parent['Max'])
	child = mesh.children[0]
	file.write('\n   ChildJoint: ')
	file.write('\n     Vec: '+vecToString(getVectTo(mesh, child)))
	if len(child.children) > 0:
		writeRobot(file, child.children[0])

#---------------------
class ExportMyFormat(bpy.types.Operator, ExportHelper):
	bl_idname       = "scene.yml";
	bl_label        = "Scene exporter";
	bl_options      = {'PRESET'};

	filename_ext    = ".yml";

	def execute(self, context):
		file = open(self.filepath, "w", encoding='utf8')

		path = os.path.dirname(self.filepath)
		name = os.path.splitext(os.path.split(self.filepath)[1])[0]
		if not os.path.exists(path+'\\'+name):
			os.makedirs(path+'\\'+name)
		offset = '\n'
		file.write('dirname: '+ '\\' + name+ '\\' )
		file.write('\nRobotPosition: '+vecToString(bpy.data.objects['Robot-Base'].location))
		file.write('\nRobot:')
		writeRobot(file, bpy.data.objects['Robot-Base'].children[0])
		writeScene(file, bpy.data.objects, offset, path, name)
		writeLamps(file, bpy.data.objects, offset)

		file.close()
		return {'FINISHED'};

def menu_func(self, context):
	self.layout.operator(ExportMyFormat.bl_idname, text="Scene (.yml)");
def register():
	bpy.utils.register_module(__name__);
	bpy.types.INFO_MT_file_export.append(menu_func);
def unregister():
	bpy.utils.unregister_module(__name__);
	bpy.types.INFO_MT_file_export.remove(menu_func);
if __name__ == "__main__":
	register()
