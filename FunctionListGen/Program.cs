namespace FunctionListGen;
class Program
{
	/// <summary>
	/// 
	/// </summary>
	/// <param name="outDir">输出目录</param>
	/// <param name="dllFileName">dll文件名，如winmm</param>
	static void Main(string outDir, string dllFileName)
	{
		char[] whitespaces = { ' ', '\t' };

		Console.WriteLine("Hello, World!");
		Directory.CreateDirectory(outDir);
		EmitDef def = new(Path.GetFileNameWithoutExtension(dllFileName));
		EmitTrunk trunk = new();

		List<IEntryVisitor> visitors = new() { def, trunk };
		// List<IEmitter> emitters = new() { def, trunk };
		string? line;
		while ((line = Console.ReadLine()) != null)
		{
			if (line != null)
			{
				var parts = line.Split(whitespaces, StringSplitOptions.TrimEntries | StringSplitOptions.RemoveEmptyEntries);
				ExportItem item;

				switch (parts.Length)
				{
					case 3:
						item = new(int.Parse(parts[0]), 0, null);
						break;
					case 4:
						item = new(int.Parse(parts[0]), int.Parse(parts[1], System.Globalization.NumberStyles.HexNumber), parts[3]);
						break;
					default:
						continue;
				}

				foreach (var visitor in visitors)
					visitor.Visit(item);
			}
		}

		def.Emit(outDir);
		trunk.Emit(outDir);
	}
}