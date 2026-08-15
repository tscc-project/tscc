interface Props66 { value: number; label: string }
export const View66 = (p: Props66) => (
  <section data-i="66">
    <h2>{p.label}</h2>
    {p.value > 66 ? <strong>{p.value as number}</strong> : <span>low</span>}
  </section>
);
