interface Props59 { value: number; label: string }
export const View59 = (p: Props59) => (
  <section data-i="59">
    <h2>{p.label}</h2>
    {p.value > 59 ? <strong>{p.value as number}</strong> : <span>low</span>}
  </section>
);
