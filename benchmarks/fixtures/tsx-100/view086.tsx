interface Props86 { value: number; label: string }
export const View86 = (p: Props86) => (
  <section data-i="86">
    <h2>{p.label}</h2>
    {p.value > 86 ? <strong>{p.value as number}</strong> : <span>low</span>}
  </section>
);
