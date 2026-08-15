interface Props47 { value: number; label: string }
export const View47 = (p: Props47) => (
  <section data-i="47">
    <h2>{p.label}</h2>
    {p.value > 47 ? <strong>{p.value as number}</strong> : <span>low</span>}
  </section>
);
