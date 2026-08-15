interface Props27 { value: number; label: string }
export const View27 = (p: Props27) => (
  <section data-i="27">
    <h2>{p.label}</h2>
    {p.value > 27 ? <strong>{p.value as number}</strong> : <span>low</span>}
  </section>
);
