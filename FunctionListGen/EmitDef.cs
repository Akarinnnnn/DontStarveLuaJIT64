using System.Text;

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
		var file = new FileStream(Path.Combine(outDir, "exports.def"), FileMode.Create);
		using StreamWriter writer = new(file, Encoding.UTF8);

		writer.WriteLine($"LIBRARY {libName}");
		writer.WriteLine("EXPORTS");

		exports.Sort((a, b) => {
			if (a.Name != null && b.Name != null)
				return a.HintIndex - b.HintIndex;
			else
				return 0;
		});

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
