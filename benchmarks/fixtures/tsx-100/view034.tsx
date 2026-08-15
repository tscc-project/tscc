interface Props34 { value: number; label: string }
export const View34 = (p: Props34) => (
  <section data-i="34">
    <h2>{p.label}</h2>
    {p.value > 34 ? <strong>{p.value as number}</strong> : <span>low</span>}
  </section>
);
