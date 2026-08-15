interface Props49 { value: number; label: string }
export const View49 = (p: Props49) => (
  <section data-i="49">
    <h2>{p.label}</h2>
    {p.value > 49 ? <strong>{p.value as number}</strong> : <span>low</span>}
  </section>
);
