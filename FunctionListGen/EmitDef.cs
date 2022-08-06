namespace FunctionListGen;

public class EmitDef : IEntryVisitor, IEmitter
{
	private readonly string libName;
	private readonly List<ExportItem> exports;

	public EmitDef(string libName)
	{
		this.libName = libName;
		exports = new List<ExportItem>();
	}

	public void Visit(ExportItem item)
	{
		exports.Add(item);
	}

	public void Emit(string outDir)
	{
		var file = File.OpenWrite(Path.Combine(outDir, "exports.def"));
		using StreamWriter writer = new(file);

		writer.WriteLine($"LIBRARY {libName}");
		writer.WriteLine("EXPORTS");

		exports.Sort((a, b) => a.HintIndex - b.HintIndex);

		foreach (var export in exports)
		{
			string name = export.Name ?? $"exp{export.Ordinal}";
			writer.Write($"{name}=trunk{export.Ordinal} @{export.Ordinal}");
			
			if (export.Name == null)
				writer.Write(" NONAME");

			writer.WriteLine();
		}
	}
}
