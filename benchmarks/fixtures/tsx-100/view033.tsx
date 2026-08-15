interface Props33 { value: number; label: string }
export const View33 = (p: Props33) => (
  <section data-i="33">
    <h2>{p.label}</h2>
    {p.value > 33 ? <strong>{p.value as number}</strong> : <span>low</span>}
  </section>
);
