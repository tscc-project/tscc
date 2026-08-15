interface Props82 { value: number; label: string }
export const View82 = (p: Props82) => (
  <section data-i="82">
    <h2>{p.label}</h2>
    {p.value > 82 ? <strong>{p.value as number}</strong> : <span>low</span>}
  </section>
);
