interface Props28 { value: number; label: string }
export const View28 = (p: Props28) => (
  <section data-i="28">
    <h2>{p.label}</h2>
    {p.value > 28 ? <strong>{p.value as number}</strong> : <span>low</span>}
  </section>
);
