interface Props76 { value: number; label: string }
export const View76 = (p: Props76) => (
  <section data-i="76">
    <h2>{p.label}</h2>
    {p.value > 76 ? <strong>{p.value as number}</strong> : <span>low</span>}
  </section>
);
